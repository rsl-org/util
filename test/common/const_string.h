#pragma once
#include <cstddef>
#include <algorithm>
#include <string_view>
#include <string>

template <std::size_t N>
struct [[nodiscard]] const_string {
  char value[N + 1]{};
  constexpr static auto size = N;

  constexpr const_string() = default;

  constexpr explicit(false) const_string(char const (&literal)[N + 1]) {  // NOLINT
    std::copy(literal, literal + N, std::begin(value));
  }

  constexpr explicit const_string(std::string_view data) { std::copy(begin(data), end(data), std::begin(value)); }
  [[nodiscard]] constexpr explicit operator std::string_view() const noexcept { return std::string_view{value}; }
  [[nodiscard]] constexpr explicit operator char const*() const noexcept { return value; }

  [[nodiscard]] constexpr auto to_sv() const noexcept { return std::string_view{value}; }
  [[nodiscard]] constexpr bool empty() const noexcept { return size == 0; }

  [[nodiscard]] constexpr char const& operator[](std::size_t Idx) const { return value[Idx]; }
  friend constexpr bool operator==(const_string lhs, std::string_view rhs) noexcept { return lhs.to_sv() == rhs; }
  [[nodiscard]]constexpr std::size_t length() const { return std::char_traits<char>::length(value); }
};

template <>
struct [[nodiscard]] const_string<0> {
  constexpr static auto size = 0;

  constexpr const_string() = default;

  [[nodiscard]] constexpr explicit operator std::string_view() const noexcept { return std::string_view{}; }
  [[nodiscard]] constexpr explicit operator char const*() const noexcept { return nullptr; }

  [[nodiscard]] constexpr auto to_sv() const noexcept { return std::string_view{}; }
  [[nodiscard]] constexpr bool empty() const noexcept { return true; }

  friend constexpr bool operator==(const_string<0>, const_string<0>) noexcept { return true; }
  template <std::size_t N>
  friend constexpr bool operator==(const_string<0>, const_string<N>) noexcept { return false; }
  
  template <std::size_t N>
  friend constexpr bool operator==(const_string, std::string_view str) noexcept { return str.empty(); }
  [[nodiscard]] constexpr std::size_t length() const { return 0; }
};

template <std::size_t N>
const_string(char const (&)[N]) -> const_string<N - 1>;