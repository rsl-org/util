#pragma once
#include <meta>
#include <string>
#include <sstream>
#include <type_traits>

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
  std::ostringstream out;
  Options opts;
  bool separate     = false;
  std::size_t level = 0;

  void print_indent() {
    if (opts.indent) {
      out << "\n";
      out << std::string(2 * level, ' ');
    }
  }

  void print_separator() {
    if (separate) {
      out << ", ";
    } else {
      separate = true;
    }
    print_indent();
  }

  void increase_nesting() {
    separate = false;
    out << '{';
    ++level;
  }

  void decrease_nesting() {
    --level;
    print_indent();
    out << '}';
    separate = true;
  }

  template <typename T>
  void print_type() {
        if (opts.names == _serialize_impl::NameMode::qualified) {
      out << _serialize_impl::qualified_name<std::remove_cvref_t<T>>;
    } else if (opts.names == _serialize_impl::NameMode::fully_qualified) {
      out << _serialize_impl::fully_qualified_name<std::remove_cvref_t<T>>;
    } else {
      out << _serialize_impl::unqualified_name<std::remove_cvref_t<T>>;
    }
  }

public:
  explicit Serializer(Options opts) : opts(opts) {}

  std::string finalize() const { return out.str(); }

  template <has_members R, typename T>
  void operator()(R meta, T&& value) {
    print_separator();

    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <is_iterable R, typename T>
  void operator()(R meta, T&& value) {
    print_separator();
    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <typename R>
  void operator()(R, std::convertible_to<std::string_view> auto const& value) {
    print_separator();
    out << '"' << value << '"';
  }

  void operator()(auto, char value) {
    print_separator();
    out << '\'' << value << '\'';
  }

  void operator()(auto, bool value) {
    print_separator();
    out << (value ? "true" : "false");
  }

  template <typename R, typename T>
    requires(std::is_floating_point_v<std::remove_cvref_t<T>>)
  void operator()(R, T value) {
    print_separator();
    out << value;
    if constexpr (std::same_as<T, float>) {
      out << "F";
    } else if constexpr (std::same_as<T, long double>) {
      out << "L";
    }
  }

  template <typename R, typename T>
    requires(std::is_integral_v<std::remove_cvref_t<T>>)
  void operator()(R, T value) {
    print_separator();
    out << value;
    if constexpr (std::is_unsigned_v<T>) {
      out << "U";
    }
    using signed_t = std::make_signed_t<T>;
    if constexpr (std::same_as<signed_t, long>) {
      out << "L";
    } else if constexpr (std::same_as<signed_t, long long>) {
      out << "LL";
    }
  }
};

}  // namespace rsl::serializer::_repr_impl