#pragma once
#include <cstddef>
#include <utility>
#include <memory>
#include <experimental/meta>

namespace rsl::_impl {
template <auto... Ptrs>
struct MemberAccessor {
  template <std::size_t Idx, typename S>
  [[gnu::always_inline]]
  constexpr static decltype(auto) get(S&& storage) noexcept {
    return std::forward_like<S>(std::forward<S>(storage).[:Ptrs...[Idx]:]);
  }

  template <std::size_t Idx, typename S>
  [[gnu::always_inline]]
  constexpr static decltype(auto) get_addr(S&& storage) noexcept {
    return std::addressof(std::forward<S>(storage).[:Ptrs...[Idx]:]);
  }
};

template <auto... Ptrs>
constexpr inline MemberAccessor<Ptrs...> member_cache{};

consteval auto cache_members(auto&& member_range) {
  std::vector<std::meta::info> members;
  for (auto member : member_range) {
    members.push_back(std::meta::reflect_value(member));
  }
  return substitute(^^member_cache, members);
}
}  // namespace rsl::_impl