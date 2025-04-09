#pragma once
#include <string>
#include <concepts>
#include <cstdint>

namespace rsl::_impl {

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

constexpr std::string to_string(std::integral auto value){
  if constexpr (std::is_signed_v<decltype(value)>){
    if (value < 0){
      return std::string{'-'} + utos(-value);
    }
  }
  return utos(value);
}
}

