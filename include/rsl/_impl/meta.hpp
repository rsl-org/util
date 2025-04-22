#pragma once

namespace rsl::_impl {
template <typename T, T... Vs>
constexpr inline T fixed_array[sizeof...(Vs)]{Vs...};
}  // namespace rsl::_impl