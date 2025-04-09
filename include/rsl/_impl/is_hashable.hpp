#pragma once
#include <type_traits>
#include <functional>

namespace rsl::_impl {
template <typename T>
concept is_hashable = 
    std::is_default_constructible_v<std::hash<T>>
    && std::is_copy_constructible_v<std::hash<T>>
    && std::is_move_constructible_v<std::hash<T>>
    && std::is_invocable_r_v<std::size_t, std::hash<T>, T const&>;
}