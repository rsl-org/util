#pragma once
#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>

#include <rsl/meta_traits>

namespace rsl::_serialize_impl {
struct Name {
  char const* data;
};

consteval Name canonical_name(std::string_view name) {
  return Name{define_static_string(name)};
}

namespace _impl {
template <typename T>
consteval std::string get_canonical_recurse();
template <typename T>
consteval bool canonical_is_overridden();

template <typename T>
consteval std::string get_canonical_name(std::meta::info recurse) {
  if constexpr (canonical_is_overridden<T>()) {
    return get_canonical_recurse<T>();
  } else if constexpr (is_enumerable_type(^^T) && meta::has_annotation<Name>(^^T)) {
    return std::string([:constant_of(annotations_of(^^T, ^^Name)[0]):].data);
  } else if constexpr (requires {
                         { T::canonical_name } -> std::convertible_to<std::string>;
                       }) {
    return std::string(T::canonical_name);
  } else if constexpr (requires {
                         { T::canonical_name() } -> std::convertible_to<std::string>;
                       }) {
    return std::string(T::canonical_name());
  } else if constexpr (requires(std::type_identity<T> id) {
                         { canonical_name(id) } -> std::convertible_to<std::string>;
                       }) {
    return std::string(canonical_name(std::type_identity<T>()));
  } else if constexpr (has_template_arguments(^^T)) {
    auto templ   = template_of(^^T);
    auto args    = template_arguments_of(^^T);
    int required = int(args.size()) - 1;
    for (; required >= 0; --required) {
      if (!can_substitute(templ, args | std::views::take(required))) {
        break;
      }
    }
    ++required;
    std::string ret;
    if (has_identifier(templ)) {
      ret = std::string(identifier_of(templ));
    } else {
      ret = "unnamed-template";
    }
    ret += "<";
    bool first = true;
    for (auto arg : args | std::views::take(required)) {
      if (first) {
        first = false;
      } else {
        ret += ", ";
      }
      ret += extract<std::string (*)()>(substitute(recurse, {arg}))();
    }
    ret += ">";
    return ret;
  } else {
    return std::string(display_string_of(^^T));
  }
}
}  // namespace _impl

template <typename T>
constexpr inline auto canonical_name_of = define_static_string(
    _impl::get_canonical_name<std::remove_cvref_t<T>>(^^_impl::get_canonical_recurse));

namespace _impl {
template <typename T>
consteval bool canonical_is_overridden() {
  return requires { canonical_name_of<T>; };
}

template <typename T>
consteval std::string get_canonical_recurse() {
  // need to make sure user-provided specializations are converted to strings
  return std::string(canonical_name_of<T>);
}
}  // namespace _impl

template <typename T>
constexpr inline auto canonical_name_of<T const> =
    define_static_string(std::string(canonical_name_of<T>) + " const");

template <typename T>
constexpr inline auto canonical_name_of<T const&> =
    define_static_string(std::string(canonical_name_of<T>) + " const&");

template <typename T>
constexpr inline auto canonical_name_of<T const&&> =
    define_static_string(std::string(canonical_name_of<T>) + " const&&");

template <typename T>
constexpr inline auto canonical_name_of<T&> =
    define_static_string(std::string(canonical_name_of<T>) + "&");

template <typename T>
constexpr inline auto canonical_name_of<T&&> =
    define_static_string(std::string(canonical_name_of<T>) + "&&");

template <typename T>  // TODO require T is not a function pointer
constexpr inline auto canonical_name_of<T*> =
    define_static_string(std::string(canonical_name_of<T>) + "*");

template <typename T>  // TODO require T is not a function pointer
constexpr inline auto canonical_name_of<T* const> =
    define_static_string(std::string(canonical_name_of<T>) + "* const");

template <>
constexpr inline auto canonical_name_of<std::string_view> = "string_view";

template <>
constexpr inline auto canonical_name_of<std::string> = "string";

namespace _impl {
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

template <typename T>
consteval std::string get_qualified_recurse();

template <typename T>
consteval std::string get_fully_qualified_recurse();
}  // namespace _impl

template <typename T>
constexpr inline auto qualified_name_of =
    define_static_string(_impl::namespace_prefix<std::remove_cvref_t<T>>() + _impl::get_canonical_name<T>(^^_impl::get_qualified_recurse));

template <typename T>
constexpr inline auto fully_qualified_name_of = define_static_string(
    "::" + _impl::namespace_prefix<std::remove_cvref_t<T>>() + _impl::get_canonical_name<T>(^^_impl::get_fully_qualified_recurse));

template <typename T>
  requires std::is_fundamental_v<T>
constexpr inline auto fully_qualified_name_of<T> = canonical_name_of<T>;

consteval std::string_view canonical_name(std::meta::info R) {
  if (is_type(R)) {
    return define_static_string(extract<std::string (*)()>(substitute(^^_impl::get_canonical_recurse, {R}))());
  } else {
    // TODO collapse t.operator() to t()?
    return display_string_of(R);
  }
}

consteval std::string_view qualified_name(std::meta::info R) {
  return define_static_string(extract<std::string(*)()>(substitute(^^_impl::get_qualified_recurse, {R}))());
}

consteval std::string_view fully_qualified_name(std::meta::info R) {
  return define_static_string(extract<std::string(*)()>(substitute(^^_impl::get_fully_qualified_recurse, {R}))());
}

namespace _impl {
  template <typename T>
consteval std::string get_qualified_recurse() {
  // need to make sure user-provided specializations are converted to strings
  return std::string(qualified_name_of<T>);
}

  template <typename T>
consteval std::string get_fully_qualified_recurse() {
  // need to make sure user-provided specializations are converted to strings
  return std::string(fully_qualified_name_of<T>);
}
}
}  // namespace rsl::_serialize_impl