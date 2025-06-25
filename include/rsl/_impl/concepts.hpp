#pragma once
#include <utility>
#include <tuple>
#include <type_traits>
#include <concepts>
#include <meta>

namespace rsl::_impl {
template <typename T>
concept is_subscriptable = requires(T obj) { obj[0]; };

template <typename T>
using subscript_result = decltype(std::declval<T>()[0]);

template <typename T>
concept is_tuple_like = requires(T obj) {
  typename std::tuple_element_t<0, std::remove_cvref_t<T>>;
  { get<0>(obj) } -> std::convertible_to<std::tuple_element_t<0, std::remove_cvref_t<T>>>;
};

template <typename T, std::meta::info Template>
concept is_specialization = has_template_arguments(^^T) && template_of(^^T) == (is_template(Template) ? Template : template_of(Template));

template<typename T>
concept is_void = std::is_void_v<T>;

template<typename T>
concept is_non_void = !is_void<T>;

template <std::meta::info R>
concept is_function = std::meta::is_function(R);

template <std::meta::info R>
concept is_member_function = is_function<R> && is_class_member(R);

template <std::meta::info R>
concept is_static_member_function = is_member_function<R> && is_static_member(R);

template <std::meta::info R>
concept is_nonstatic_member_function = is_member_function<R> && !is_static_member(R);
}  // namespace erl::util