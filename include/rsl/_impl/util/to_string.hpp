#pragma once
#include <string>
#include <concepts>
#include <cstdint>
#include <meta>

namespace rsl::util {

constexpr std::string to_string(bool value) {
  return value ? "true" : "false";
}

constexpr std::string to_string(char value) {
  return {value, 1};
}

constexpr std::string utos(std::uint64_t value) {
  std::string out{};
  do {
    out += static_cast<char>('0' + (value % 10U));
    value /= 10;
  } while (value > 0);
  return {out.rbegin(), out.rend()};
}

constexpr std::uint64_t stou(std::string_view str) {
  unsigned result = 0;
  for (char const c : str) {
    (result *= 10) += c - '0';
  }
  return result;
}

constexpr std::string to_string(std::integral auto value) {
  if constexpr (std::is_signed_v<decltype(value)>) {
    if (value < 0) {
      return std::string{'-'} + utos(-value);
    }
  }
  return utos(value);
}

consteval std::string to_string(std::meta::info value) {
  // TODO this is incorrect for reflections of values
  return std::string("^^") + display_string_of(value);
}
}  // namespace rsl::util
