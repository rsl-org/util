#pragma once
#include <cstddef>
#include <string>
#include <type_traits>
#include <meta>

#include <rsl/serialize>
#include <rsl/meta_traits>
#include <rsl/string_view>

#include <rsl/stream/stream.hpp>
#include "rsl/serializer/machinery.hpp"

namespace rsl::serializer {

class Binary {
  stream::OutputStream* stream;
  
public:
  stream::IOStatus status = stream::IOStatus::ok;
  constexpr explicit Binary(stream::OutputStream& stream) : stream(&stream) {}

  constexpr void finalize() {
    // TODO flush stream
  }

  template <typename T>
  constexpr void operator()(Unsupported, T&& value) {
    status = stream::IOStatus::unsupported;
  }

  template <has_members R, typename T>
  constexpr void operator()(R meta, T&& value) {
    meta.descend(*this, std::forward<T>(value));
  }

  template <is_iterable R, typename T>
  constexpr void operator()(R meta, T&& value) {
    auto data = std::vector(value);
    serialize_integral(data.size());
    Meta<decltype(data)>{}.descend(*this, data);
  }

  template <typename R>
  constexpr void operator()(R, std::convertible_to<std::string_view> auto const& value) {
    std::string_view val = value;
    serialize_integral(val.size());
    stream->write(reinterpret_cast<std::byte const*>(val.data()), val.size());
  }

  template <typename T, typename U>
  constexpr void operator()(auto meta, std::pair<T, U> const& pair) {
    meta.descend(*this, pair);
  }

  template <typename R, typename T>
    requires std::is_floating_point_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    // TODO
  }

  template <typename R, typename T>
    requires std::is_integral_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    serialize_integral(value);
  }

  template <typename T>
    requires std::is_integral_v<std::remove_cvref_t<T>>
  constexpr void serialize_integral(T value) {
    stream->write(reinterpret_cast<std::byte const*>(&value), sizeof(T));
  }
};

template <typename T>
struct DeserializationResult {
  std::optional<T> value;
  stream::IOResult status;
};

class BinaryDeserializer {
  stream::InputStream* stream;
  
public:
  stream::IOStatus status = stream::IOStatus::ok;
  constexpr explicit BinaryDeserializer(stream::InputStream& stream) : stream(&stream) {}

  constexpr void operator()(Unsupported) { }

  template <typename T>
    requires has_members<Meta<T>>
  constexpr DeserializationResult<T> operator()(Meta<T> meta) {
    // meta.descend(*this, std::forward<T>(value));
  }

  template <typename T>
    requires is_iterable<Meta<T>>
  constexpr DeserializationResult<T> operator()(Meta<T> meta) {
    
  }

  template <std::convertible_to<std::string_view> T>
    requires is_iterable<Meta<T>>
  constexpr DeserializationResult<T> operator()(Meta<T> meta) {
    
  }

  template <typename T, typename U>
  constexpr DeserializationResult<std::pair<T, U>> operator()(Meta<std::pair<T, U>> meta) {
    
  }

  template <typename T>
    requires std::is_floating_point_v<std::remove_cvref_t<T>>
  constexpr DeserializationResult<T> operator()(Meta<T> meta) {
    // TODO
  }

  template <typename T>
    requires std::is_integral_v<T>
  constexpr DeserializationResult<T> operator()(Meta<T>) {
    return deserialize_integral<T>();
  }

  template <typename T>
    requires std::is_integral_v<std::remove_cvref_t<T>>
  constexpr DeserializationResult<T> deserialize_integral() {
    std::array<std::byte, sizeof(T)> data;
    auto result = stream->read(&data, sizeof(T));
    if(result.status != stream::IOStatus::ok) {
      status = result;
    }
    // TODO this is unsafe
    return {*reinterpret_cast<T*>(&data), result};
  }
};

}  // namespace rsl::serializer