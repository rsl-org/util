#pragma once
#include <cstddef>
#include <string>
#include <type_traits>
#include <meta>
#include <algorithm>
#include <charconv>

#include <rsl/serialize>
#include <rsl/meta_traits>
#include <rsl/string_view>

#include "name.hpp"

namespace rsl::serializer {


struct Options {
  NameMode names = NameMode::unqualified;
  bool indent    = false;
};

class ReprVisitor {
  std::string out;
  Options opts;
  bool separate     = false;
  std::size_t level = 0;

  constexpr void print_indent() {
    if (opts.indent) {
      out += '\n';
      out.append(2 * level, ' ');
    }
  }

  constexpr void print_separator() {
    if (separate) {
      out += ", ";
    } else {
      separate = true;
    }
    print_indent();
  }

  constexpr void increase_nesting() {
    separate = false;
    out += '{';
    ++level;
  }

  constexpr void decrease_nesting() {
    --level;
    print_indent();
    out += '}';
    separate = true;
  }

  template <typename T>
  constexpr void print_type() {
    using CleanT = std::remove_cvref_t<T>;
    switch (opts.names) {
      case NameMode::qualified: out += qualified_name<CleanT>; break;
      case NameMode::fully_qualified: out += fully_qualified_name<CleanT>; break;
      case NameMode::unqualified: out += unqualified_name<CleanT>; break;
    }
  }

public:
  constexpr explicit ReprVisitor(Options opts = {}) : opts(opts) {}

  constexpr std::string finalize() const { return out; }

  template <typename T>
  constexpr void operator()(Unsupported, T&& value) {
    print_separator();
    print_type<T>();
    increase_nesting();
    out += "/*...*/";
    decrease_nesting();
  }

  template <has_members R, typename T>
  constexpr void operator()(R meta, T&& value) {
    print_separator();
    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <is_iterable R, typename T>
  constexpr void operator()(R meta, T&& value) {
    print_separator();
    print_type<std::remove_cvref_t<T>>();
    increase_nesting();
    meta.descend(*this, std::forward<T>(value));
    decrease_nesting();
  }

  template <typename R>
  constexpr void operator()(R, std::convertible_to<std::string_view> auto const& value) {
    print_separator();
    out += '"';
    out += std::string(value);  // allowed in C++26 constexpr
    out += '"';
  }

  constexpr void operator()(auto, char value) {
    print_separator();
    out += '\'';
    out += value;
    out += '\'';
  }

  constexpr void operator()(auto, bool value) {
    print_separator();
    out += value ? "true" : "false";
  }

  template <typename T, typename U>
  constexpr void operator()(auto meta, std::pair<T, U> const& pair) {
    print_separator();
    increase_nesting();
    meta.descend(*this, pair);
    decrease_nesting();
  }

  template <typename R, typename T>
    requires std::is_floating_point_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    print_separator();
    std::array<char, 64> buffer{};

    auto [ptr, ec] = std::to_chars(buffer.data(),
                                   buffer.data() + buffer.size(),
                                   value,
                                   std::chars_format::general);
    if (ec != std::errc{}) {
      out += "/* error */";
      return;
    }

    std::string result(buffer.data(), ptr - buffer.data());

    if constexpr (std::same_as<T, float>) {
      result += 'F';
    } else if constexpr (std::same_as<T, long double>) {
      result += 'L';
    }
    out += result;
  }

  template <typename R, typename T>
    requires std::is_integral_v<std::remove_cvref_t<T>>
  constexpr void operator()(R, T value) {
    print_separator();
    std::array<char, 32> buffer{};  // Enough for any 64-bit integer including sign
    auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

    if (ec != std::errc{}) {
      out += "/* error */";
      return;
    }
    out += std::string(buffer.data(), ptr - buffer.data());

    if constexpr (std::is_unsigned_v<T>) {
      out += 'U';
    }
    using Signed = std::make_signed_t<T>;
    if constexpr (std::same_as<Signed, long>) {
      out += 'L';
    } else if constexpr (std::same_as<Signed, long long>) {
      out += "LL";
    }
  }
};

namespace _impl {
template <std::meta::info R, NameMode mode>
consteval std::string stringify_constant_impl() {
  auto visitor = ReprVisitor{{.names = mode}};
  rsl::serialize(visitor, [:R:]);
  return visitor.finalize();
}

consteval std::string stringify_constant(std::meta::info R, NameMode mode = NameMode::unqualified) {
  return extract<std::string (*)()>(
      substitute(^^stringify_constant_impl,
                 {reflect_constant(R), std::meta::reflect_constant(mode)}))();
}
}

consteval std::string_view stringify_template_args(std::meta::info R,
                                              NameMode mode          = NameMode::unqualified,
                                              bool discard_defaulted = true) {
  std::string ret = "<";
  auto args       = template_arguments_of(R);
  int required    = int(args.size()) - 1;
  if (discard_defaulted) {
    for (; required >= 0; --required) {
      if (!can_substitute(template_of(R), args | std::views::take(required))) {
        break;
      }
    }
  }
  ++required;

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
    } else if (is_value(arg) || is_object(arg)) {
      ret += _impl::stringify_constant(arg, mode);
    } else {
      ret += display_string_of(arg);
    }
  }
  return define_static_string(ret + ">");
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
  if constexpr (is_enumerable_type(^^T) && rsl::meta::has_annotation<preferred_name<_impl::Annotated>>(^^T)) {
    return ret + [:constant_of(annotations_of(^^T, ^^preferred_name<_impl::Annotated>)[0]):].value;
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
    auto templ = template_of(^^T);

    if (has_identifier(templ)) {
      ret += std::string(identifier_of(templ));
    } else {
      ret += "unnamed-template";
    }
    ret += stringify_template_args(^^T, mode);
    return ret;
  } else {
    return ret + display_string_of(^^T);
  }
}
}  // namespace _impl

}  // namespace rsl::serializer