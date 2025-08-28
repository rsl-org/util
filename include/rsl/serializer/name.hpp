#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <meta>
#include <concepts>
#include <type_traits>
#include <vector>

#include <rsl/meta_traits>
#include <rsl/string_view>

namespace rsl::serializer {
namespace _impl {
template <typename T>
consteval std::string get_via_adl() {
  if constexpr (requires(std::type_identity<T> id) {
                  { preferred_name(id) } -> std::convertible_to<std::string>;
                }) {
    return std::string(preferred_name(std::type_identity<T>()));
  }
  return "";
}

struct Annotated {};
}  // namespace _impl

template <typename T>
struct preferred_name;

template <>
struct preferred_name<_impl::Annotated> {
  rsl::string_view value;
  consteval explicit preferred_name(std::string_view name) : value(define_static_string(name)) {}
};

template <std::convertible_to<std::string_view> T>
preferred_name(T&&) -> preferred_name<_impl::Annotated>;

template <>
struct preferred_name<std::string> {
  constexpr static auto value = "string";
};

template <>
struct preferred_name<std::string_view> {
  constexpr static auto value = "string_view";
};

enum class NameMode : std::uint8_t { unqualified, qualified, fully_qualified };

namespace _impl {
template <typename T>
consteval std::string get_type_name(NameMode mode);

consteval std::vector<std::string_view> get_fully_qualified_name(std::meta::info R) {
  std::vector<std::string_view> name{identifier_of(R)};

  std::ranges::reverse(name);
  return name;
}

template <typename T>
consteval std::string namespace_prefix() {
  if constexpr (std::is_fundamental_v<T>) {
    return "";
  }

  std::string ret;
  auto current = ^^T;
  while (meta::has_parent(current)) {
    current = parent_of(current);
    if (!has_identifier(current) || identifier_of(current).starts_with("__")) {
      // do not print namespaces with reserved identifiers
      // this is a hack, but it cleans up standard library names a lot
      continue;
    }
    ret.insert(0, std::string(identifier_of(current)) + "::");
  }

  return ret;
}
}  // namespace _impl

template <typename T, NameMode Mode = NameMode::unqualified>
constexpr inline std::string_view type_name = define_static_string(_impl::get_type_name<T>(Mode));

template <typename T>
constexpr inline std::string_view unqualified_name = type_name<T, NameMode::unqualified>;

template <typename T>
constexpr inline std::string_view qualified_name = type_name<T, NameMode::qualified>;

template <typename T>
constexpr inline std::string_view fully_qualified_name = type_name<T, NameMode::fully_qualified>;

template <typename T, NameMode Mode>
constexpr inline std::string_view type_name<T const, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + " const");

template <typename T, NameMode Mode>
constexpr inline std::string_view type_name<T const&, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + " const&");

template <typename T, NameMode Mode>
constexpr inline std::string_view type_name<T const&&, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + " const&&");

template <typename T, NameMode Mode>
constexpr inline std::string_view type_name<T&, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + "&");

template <typename T, NameMode Mode>
constexpr inline std::string_view type_name<T&&, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + "&&");

template <typename T, NameMode Mode>  // TODO require T is not a function pointer
constexpr inline std::string_view type_name<T*, Mode> =
    define_static_string(std::string(type_name<T, Mode>) + "*");

consteval std::string_view name_of(std::meta::info R, NameMode mode = NameMode::unqualified) {
  if (is_type(R)) {
    // return define_static_string(extract<std::string (*)()>(
    //     substitute(^^_impl::get_canonical_recurse, {R, std::meta::reflect_constant(mode)}))());
    return define_static_string(
        extract<std::string_view>(substitute(^^type_name, {R, std::meta::reflect_constant(mode)})));
  } else {
    // TODO handle qualified/fully_qualified
    // TODO collapse t.operator() to t()?
    return display_string_of(R);
  }
}

consteval std::string_view unqualified_name_of(std::meta::info R) {
  return name_of(R, NameMode::unqualified);
}

consteval std::string_view qualified_name_of(std::meta::info R) {
  return name_of(R, NameMode::qualified);
}

consteval std::string_view fully_qualified_name_of(std::meta::info R) {
  return name_of(R, NameMode::fully_qualified);
}
}  // namespace rsl::serializer