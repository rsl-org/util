#pragma once
#include <meta>
#include <ranges>
#include <rsl/meta_traits>
#include "annotations.hpp"

namespace rsl::serializer {

template <typename T>
concept is_meta = requires {
  { T::info } -> std::convertible_to<std::meta::info>;
};

template <typename T>
concept has_members = is_meta<T> && requires {
  { T::members } -> std::convertible_to<std::span<std::meta::info const>>;
};

template <typename T>
concept is_iterable = is_meta<T> && T::iterable;

template <typename T>
concept is_optional = is_meta<T> && T::is_optional;

struct Unsupported {};

template <typename T>
struct Meta : Unsupported {
  static constexpr std::meta::info info = ^^T;

  template <typename U, typename F>
    requires(std::same_as<std::remove_cvref_t<U>, T>)
  constexpr void descend(F&& visitor, U&& value) {}
};

template <typename T>
  requires(std::is_scalar_v<T> && !std::is_pointer_v<T>)
struct Meta<T> {
  static constexpr std::meta::info info = ^^T;

  template <typename U, typename F>
    requires(std::same_as<std::remove_cvref_t<U>, T>)
  constexpr void descend(F&& visitor, U&& value) {}
};

template <std::size_t Idx, std::meta::info R, typename T>
struct Member : Meta<T> {
  static constexpr std::meta::info info = R;
  static constexpr std::size_t index    = Idx;
};

template <typename T>
  requires(std::is_aggregate_v<T> && !std::is_array_v<T>)
struct Meta<T> {
  static constexpr std::meta::info info = ^^T;
  static constexpr auto members =
      define_static_array(nonstatic_data_members_of(info, std::meta::access_context::current()));

  template <typename F, typename U>
    requires(std::same_as<std::remove_cvref_t<U>, T>)
  constexpr void descend(F&& visitor, U&& value) {
    template for (constexpr auto Idx : std::views::iota(0ZU, members.size())) {
      constexpr auto M = members[Idx];
      if constexpr (!meta::has_annotation(M, ^^annotations::Skip)) {
        std::invoke(visitor, Member<Idx, M, typename[:type_of(M):]>{}, value.[:M:]);
      }
    }
  }
};

template <std::ranges::range T>
  requires(std::constructible_from<std::initializer_list<typename T::value_type>> &&
           !std::convertible_to<T, std::string_view>)
struct Meta<T> {
  using type                            = T;
  using element_type                    = typename T::value_type;
  constexpr static std::meta::info info = ^^T;
  constexpr static bool can_descend     = true;
  constexpr static bool iterable        = true;
  constexpr static bool associative     = requires {
    typename type::key_type;
    typename type::mapped_type;
  };

  template <typename F, typename U>
    requires(std::same_as<std::remove_cvref_t<U>, T>)
  constexpr void descend(F&& visitor, U&& value) {
    for (auto&& item : value) {
      std::invoke(visitor, Meta<std::remove_cvref_t<decltype(item)>>{}, item);
    }
  }
};

template <typename First, typename Second>
struct Meta<std::pair<First, Second>> {
  using type                            = std::pair<First, Second>;
  constexpr static std::meta::info info = dealias(^^type);

  template <typename S, typename F, typename U>
    requires(std::same_as<std::remove_cvref_t<U>, type>)
  constexpr void descend(this S&& self, F&& visitor, U&& value) {
    std::invoke(std::forward<F>(visitor), Meta<std::remove_cvref_t<First>>{}, value.first);
    std::invoke(std::forward<F>(visitor), Meta<std::remove_cvref_t<Second>>{}, value.second);
  }
};

template <typename T>
struct Meta<std::optional<T>> {
  using type                            = std::optional<T>;
  using element_type                    = T;
  constexpr static std::meta::info info = dealias(^^type);
  constexpr static bool is_optional     = true;

  template <typename S, typename F, typename U>
    // requires(std::same_as<std::remove_cvref_t<U>, type>)
  constexpr void descend(this S&& self, F&& visitor, U&& value) {
    if (value.has_value()) {
      std::invoke(std::forward<F>(visitor), Meta<std::remove_cvref_t<T>>{}, *value);
    }
  }
};

template <std::convertible_to<std::string_view> T>
struct Meta<T> {
  using type                            = T;
  constexpr static std::meta::info info = ^^T;

  template <typename S, typename F, typename U>
    requires(std::same_as<std::remove_cvref_t<U>, type>)
  constexpr void descend(this S&& self, F&& visitor, U&& value) {}
};

}  // namespace rsl::serializer