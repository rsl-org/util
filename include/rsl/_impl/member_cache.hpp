#pragma once
#include <cstddef>
#include <utility>
#include <memory>
#include <experimental/meta>

namespace rsl::_impl {
template <auto... Members>
struct MemberAccessor {
  template <std::size_t Idx, typename S>
  [[gnu::always_inline]]
  constexpr static decltype(auto) get(S&& storage) noexcept {
    return std::forward_like<S>(storage.[:Members...[Idx]:]);
  }

  template <std::size_t Idx, typename S>
  [[gnu::always_inline]]
  constexpr static decltype(auto) get_addr(S&& storage) noexcept {
    return std::addressof(std::forward<S>(storage).[:Members...[Idx]:]);
  }

  constexpr static std::array<std::meta::info, sizeof...(Members)> types = {dealias(type_of(Members))...};
  constexpr static auto count = sizeof...(Members);

  static consteval std::size_t get_index_of(std::meta::info needle) {
    constexpr std::size_t npos = ~0ZU;
    std::size_t selected       = npos;
    for (std::size_t idx = 0; idx < types.size(); ++idx) {
      if (types[idx] != needle) {
        continue;
      }

      if (selected != npos) {
        return npos;
      }

      selected = idx;
    }
    return selected;
  }
};

template <auto... Members>
constexpr inline MemberAccessor<Members...> member_cache{};

consteval auto cache_members(auto&& member_range) {
  std::vector<std::meta::info> members;
  for (auto member : member_range) {
    members.push_back(std::meta::reflect_constant(member));
  }
  return substitute(^^member_cache, members);
}
}  // namespace rsl::_impl