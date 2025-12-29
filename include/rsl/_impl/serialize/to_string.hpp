#pragma once
#include <string>
#include <system_error>
#include "rsl/enum"

namespace rsl::_impl_serialize {
constexpr std::string to_string(int value);
constexpr std::string to_string(long value);
constexpr std::string to_string(long long value);
constexpr std::string to_string(unsigned value);
constexpr std::string to_string(unsigned long value);
constexpr std::string to_string(unsigned long long value);

// can these be constexpr? fpenv?
std::string to_string(float value);
std::string to_string(double value);
std::string to_string(long double value);

// non-standard
constexpr std::string to_string(bool value) {
  return value ? "true" : "false";
}

template <typename T>
  requires std::is_enum_v<T>
constexpr std::string to_string(T value) {
  template for (constexpr auto E : define_static_array(enumerators_of(^^T))) {
    if (extract<T>(constant_of(E)) == value) {
      // at least one enumerator was an exact match, print the first
      return std::string{identifier_of(E)};
    }
  }

  // no enumerator matched exactly
  if constexpr (is_flag_enum<T>) {
    // for flag-likes we want to support `A | B` unless we got exact match
    
  }

  // still no match, print as cast using the functional notation
  return std::string(identifier_of(^^T)) + "(" + to_string(std::to_underlying(value)) + ")";
}

constexpr std::string to_string(char value) {
  return {value};
}

constexpr std::string to_string(std::string_view value) {
  return std::string{value};
}
}  // namespace rsl::_impl_serialize