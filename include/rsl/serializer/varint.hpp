#pragma once

#include <concepts>
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <limits>

#include <rsl/stream/stream.hpp>

namespace rsl::serializer {
template <std::integral T>
struct VarInt {
  using value_type    = T;
  using unsigned_type = std::make_unsigned_t<T>;
  using signed_type   = std::make_signed_t<T>;

  static constexpr std::size_t size(T v) noexcept {
    if constexpr (std::signed_integral<T>) {
      return size_unsigned(zigzag_encode(v));
    } else {
      return size_unsigned(static_cast<unsigned_type>(v));
    }
  }

  static std::size_t encode(T v, stream::OutputStream& out) noexcept {
    if constexpr (std::signed_integral<T>) {
      return encode_unsigned(zigzag_encode(v), out);
    } else {
      return encode_unsigned(static_cast<unsigned_type>(v), out);
    }
  }

  static stream::IOResult decode(stream::InputStream& stream, T& out) noexcept {
    unsigned_type tmp{};
    auto io_result = decode_unsigned(stream, tmp);
    if (!io_result.status != stream::IOStatus::ok) {
      return io_result;
    }

    if constexpr (std::signed_integral<T>) {
      out = zigzag_decode(tmp);
    } else {
      out = static_cast<T>(tmp);
    }
    return io_result;
  }

private:
  static constexpr unsigned_type zigzag_encode(signed_type value) noexcept
    requires std::signed_integral<T>
  {
    constexpr int bits = std::numeric_limits<signed_type>::digits;
    return (static_cast<unsigned_type>(value) << 1) ^ static_cast<unsigned_type>(value >> (bits - 1));
  }

  static constexpr signed_type zigzag_decode(unsigned_type value) noexcept
    requires std::signed_integral<T>
  {
    return static_cast<signed_type>((value >> 1) ^ (~(value & 1) + 1));
  }

  static constexpr std::size_t size_unsigned(unsigned_type value) noexcept {
    std::size_t n = 1;
    while (value >= 0x80) {
      value >>= 7;
      ++n;
    }
    return n;
  }

  static stream::IOResult encode_unsigned(unsigned_type value, stream::OutputStream& out) noexcept {
    std::uint8_t current = {};
    std::size_t written  = 0;
    while (value >= 0x80) {
      current          = static_cast<std::uint8_t>(value) | 0x80U;
      auto [_, status] = out.write(reinterpret_cast<std::byte*>(&current), 1);
      if (status != stream::IOStatus::ok) {
        return {written, status};
      }
      written++;
      value >>= 7;
    }
    current          = static_cast<std::uint8_t>(value);
    auto [_, status] = out.write(reinterpret_cast<std::byte*>(&current), 1);
    if (status != stream::IOStatus::ok) {
      return {written, status};
    }
    written++;
    return {written, stream::IOStatus::ok};
  }

  static stream::IOResult decode_unsigned(stream::InputStream& stream,
                                          unsigned_type& out) noexcept {
    constexpr unsigned max_len = (std::numeric_limits<unsigned_type>::digits + 6) / 7;
    out                        = 0;
    unsigned shift             = 0;
    std::size_t read_count     = 1;
    std::uint8_t current       = {};

    for (unsigned i = 0; i < max_len; ++i) {
      auto io_result = stream.read(reinterpret_cast<std::byte*>(&current), 1);
      ++read_count;
      if (io_result.status != stream::IOStatus::ok) {
        return {read_count, io_result.status};
      }

      out |= static_cast<unsigned_type>(current & 0x7FU) << shift;
      if ((current & 0x80U) == 0) {
        return {read_count, stream::IOStatus::ok};
      }
      shift += 7;
    }
    return {read_count, stream::IOStatus::decode_error};
  }
};
}  // namespace rsl::serializer