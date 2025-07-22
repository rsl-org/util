#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>

#include <rsl/meta_traits>
#include <rsl/string_view>

namespace rsl::_serialize_impl {

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
}  // namespace _impl

template <typename T>
struct preferred_name;

template <>
struct preferred_name<void> {
  constexpr static auto value = "void";
  char const* data            = nullptr;
  consteval explicit preferred_name(std::string_view name) : data(define_static_string(name)) {}
};

template <std::convertible_to<std::string_view> T>
preferred_name(T&&) -> preferred_name<void>;

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
    return define_static_string(extract<std::string_view>(
        substitute(^^type_name, {R, std::meta::reflect_constant(mode)})));
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

namespace _impl {
template <typename T>
consteval std::string get_type_name(NameMode mode) {
  if constexpr (std::is_fundamental_v<T>) {
    return std::string(display_string_of(^^T));
  }

  std::string ret;
  if (mode == NameMode::fully_qualified) {
    ret += "::";
  }
  if (mode != NameMode::unqualified) {
     ret += _impl::namespace_prefix<std::remove_cvref_t<T>>();
  }

  if constexpr (rsl::meta::complete_type<preferred_name<T>>) {
    return ret + preferred_name<T>::value;
  }
  if constexpr (is_enumerable_type(^^T) && rsl::meta::has_annotation<preferred_name<void>>(^^T)) {
    return ret + [:constant_of(annotations_of(^^T, ^^preferred_name<void>)[0]):].data;
  }
  if constexpr (requires {
                  { T::preferred_name } -> std::convertible_to<std::string>;
                }) {
    return ret + T::preferred_name;
  }
  if constexpr (requires {
                  { T::preferred_name() } -> std::convertible_to<std::string>;
                }) {
    return ret + T::preferred_name();
  }
  if constexpr (requires(std::type_identity<T> id) {
                  { preferred_name(id) } -> std::convertible_to<std::string>;
                }) {
    return ret + preferred_name(std::type_identity<T>());
  }

  if (auto name = _impl::get_via_adl<T>(); !name.empty()) {
    return ret + name;
  }

  if constexpr (has_template_arguments(^^T)) {
    auto templ   = template_of(^^T);
    auto args    = template_arguments_of(^^T);
    int required = int(args.size()) - 1;
    for (; required >= 0; --required) {
      if (!can_substitute(templ, args | std::views::take(required))) {
        break;
      }
    }
    ++required;
    if (has_identifier(templ)) {
      ret += std::string(identifier_of(templ));
    } else {
      ret += "unnamed-template";
    }
    ret += "<";
    bool first = true;
    for (auto arg : args | std::views::take(required)) {
      if (first) {
        first = false;
      } else {
        ret += ", ";
      }
      if (is_type(arg)) {
        ret += extract<std::string_view>(
            substitute(^^type_name, {arg, std::meta::reflect_constant(mode)}));
      } else {
        // TODO use repr for values
        ret += display_string_of(arg);
      }
    }
    ret += ">";
    return ret;
  } else {
    return ret + display_string_of(^^T);
  }
}
}  // namespace _impl
}  // namespace rsl::_serialize_impl