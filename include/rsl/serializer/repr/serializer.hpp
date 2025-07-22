#pragma once
#include <meta>
#include <string>
#include <sstream>
#include <type_traits>
#include <charconv>

#include <rsl/serialize>
#include "../name.hpp"
#include "rsl/serializer/machinery.hpp"

namespace rsl::serializer::_repr_impl {
struct tag {};

struct Options {
  _serialize_impl::NameMode names = _serialize_impl::NameMode::unqualified;
  bool indent                     = false;
};
class Serializer {
  std::string out;
  Options opts;
  bool separate     = false;
  std::size_t level = 0;

  constexpr void print_indent() {
    if (opts.indent) {
      out += '\n';
      out.append(2 * level, ' ');
    }
  }

  constexpr void print_separator() {
    if (separate) {
      out += ", ";
    } else {
      separate = true;
    }
    print_indent();
  }

  constexpr void increase_nesting() {
    separate = false;
    out += '{';
    ++level;
  }

  constexpr void decrease_nesting() {
    --level;
    print_indent();
    out += '}';
    separate = true;
  }

  template <typename T>
  constexpr void print_type() {
    using CleanT = std::remove_cvref_t<T>;
    switch (opts.names) {
      case _serialize_impl::NameMode::qualified:
        out += _serialize_impl::qualified_name<CleanT>;
        break;
      case _serialize_impl::NameMode::fully_qualified:
        out += _serialize_impl::fully_qualified_name<CleanT>;
        break;
      case _serialize_impl::NameMode::unqualified:
        out += _serialize_impl::unqualified_name<CleanT>;
        break;
    }
  }

public:
  constexpr explicit Serializer(Options opts = {}) : opts(opts) {}

  constexpr std::string finalize() const { return out; }

  template <typename T>
  constexpr void operator()(Unsupported, T&& value) {
    print_separator();
    print_type<T>();
    increase_nesting();
    out += "/*...*/";
    decrease_nesting();
  }

  template <has_members R, typename T>
  constexpr void operator()(R meta, T&& value) {
    print_separator();
    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <is_iterable R, typename T>
  constexpr void operator()(R meta, T&& value) {
    print_separator();
    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <typename R>
  constexpr void operator()(R, std::convertible_to<std::string_view> auto const& value) {
    print_separator();
    out += '"';
    out += std::string(value);  // allowed in C++26 constexpr
    out += '"';
  }

  constexpr void operator()(auto, char value) {
    print_separator();
    out += '\'';
    out += value;
    out += '\'';
  }

  constexpr void operator()(auto, bool value) {
    print_separator();
    out += value ? "true" : "false";
  }

  template <typename T, typename U>
  constexpr void operator()(auto meta, std::pair<T, U> const& pair) {
    print_separator();
    increase_nesting();
    meta.descend(*this, pair);
    decrease_nesting();
  }

  template <typename R, typename T>
    requires std::is_floating_point_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    print_separator();
    std::array<char, 64> buffer{};

    auto [ptr, ec] = std::to_chars(buffer.data(),
                                   buffer.data() + buffer.size(),
                                   value,
                                   std::chars_format::general);
    if (ec != std::errc{}) {
      out += "/* error */";
      return;
    }

    std::string result(buffer.data(), ptr - buffer.data());

    if constexpr (std::same_as<T, float>) {
      result += 'F';
    } else if constexpr (std::same_as<T, long double>) {
      result += 'L';
    }
    out += result;
  }

  template <typename R, typename T>
    requires std::is_integral_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    print_separator();
    out += std::to_string(value);
    if constexpr (std::is_unsigned_v<T>) {
      out += 'U';
    }
    using Signed = std::make_signed_t<T>;
    if constexpr (std::same_as<Signed, long>) {
      out += 'L';
    } else if constexpr (std::same_as<Signed, long long>) {
      out += "LL";
    }
  }
};

}  // namespace rsl::serializer::_repr_impl