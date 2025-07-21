#pragma once
#include <meta>
#include <string>
#include <sstream>
#include <type_traits>

#include <rsl/serialize>

namespace rsl::serializer::_repr_impl {

class Serializer {
  std::ostringstream out;
  bool separate     = false;
  std::size_t level = 0;

  void print_indent() {
    // out << "\n";
    // out << std::string(2 * level, ' ');
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

public:
  Serializer() = default;

  std::string finalize() const {
    return out.str();
  }

  template <has_members R, typename T>
  void operator()(R meta, T&& value) {
    print_separator();

    out << identifier_of(R::info);
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <is_iterable R, typename T>
  void operator()(R meta, T&& value) {
    print_separator();
    out << define_static_string(display_string_of(type_of(R::info)));
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <typename R, typename T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>>)
  void operator()(R, T&& value) {
    print_separator();
    out << value;
  }
};

}  // namespace rsl::serializer::_repr_impl