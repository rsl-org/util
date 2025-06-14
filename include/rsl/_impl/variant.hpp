#pragma once
#include <compare>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <concepts>
#include <experimental/meta>

#include <rsl/_impl/member_cache.hpp>
#include <rsl/_impl/index_of.hpp>
#include "rsl/_impl/to_string.hpp"

namespace rsl {
template <std::size_t, typename>
struct variant_alternative;

template <typename>
struct variant_size;

namespace _variant_impl {

inline constexpr std::size_t variant_npos = -1ULL;
[[noreturn]] void throw_bad_variant_access(bool);

class bad_variant_access : public std::exception {
  friend void throw_bad_variant_access(bool);
  char const* reason = "bad variant access";
  explicit bad_variant_access(const char* message) noexcept : reason(message) {}

public:
  bad_variant_access() noexcept = default;
  [[nodiscard]] const char* what() const noexcept override { return reason; }
};

[[noreturn]] inline void throw_bad_variant_access(bool valueless) {
  __builtin_debugtrap();
#if __cpp_exceptions
  if (valueless) {
    throw bad_variant_access("variant is valueless");
  }
  throw bad_variant_access("wrong index for variant");
#else
  std::abort();
#endif
}

template <typename Source, typename Dest>
concept allowed_conversion = requires(Source obj) { std::type_identity_t<Dest[]>{std::move(obj)}; };

template <std::size_t Idx, typename T>
struct Build_FUN {
  template <allowed_conversion<T> U>
  auto operator()(T, U&&) -> std::integral_constant<std::size_t, Idx>;
};

template <typename V, typename = std::make_index_sequence<V::size>>
struct Build_FUNs;

template <template <typename...> class V, typename... Ts, std::size_t... Idx>
struct Build_FUNs<V<Ts...>, std::index_sequence<Idx...>> : Build_FUN<Idx, Ts>... {
  using Build_FUN<Idx, Ts>::operator()...;
};

template <typename T, typename V>
inline constexpr auto selected_index = variant_npos;

template <typename T, typename V>
  requires std::invocable<Build_FUNs<V>, T, T>
inline constexpr auto selected_index<T, V> = std::invoke_result_t<Build_FUNs<V>, T, T>::value;

template <typename T>
concept is_in_place = has_template_arguments(^^T) && (template_of(^^T) == ^^std::in_place_type_t ||
                                                      template_of(^^T) == ^^std::in_place_index_t);

template <typename T>
concept has_get = requires(T obj) { obj.template get<0>(); };

template <typename Operator>
struct ComparisonVisitor {
  template <typename T1, typename T2>
  constexpr bool operator()(T1&& lhs, T2&& rhs) {
    static_assert(
        std::is_convertible_v<decltype(Operator{}(std::forward<T1>(lhs), std::forward<T2>(rhs))),
                              bool>,
        "relational operator result isn't implicitly convertible to bool");
    return Operator{}(std::forward<T1>(lhs), std::forward<T2>(rhs));
  }
};
}  // namespace _variant_impl

namespace _visit_impl {
template <std::size_t... Dimensions>
struct Key {
  constexpr static std::size_t size      = sizeof...(Dimensions);
  constexpr static std::size_t max_index = (Dimensions * ... * 1);

  static consteval auto generate_offsets() {
    std::array<std::size_t, size> result = {};
    constexpr std::size_t dimensions[]   = {Dimensions...};
    result[0]                            = 1;
    for (std::size_t idx = 1; idx < size; ++idx) {
      result[idx] = dimensions[idx - 1] * result[idx - 1];
    }

    return result;
  }

  constexpr static auto offsets = generate_offsets();

  std::size_t index                             = static_cast<std::size_t>(-1);
  std::size_t subindices[sizeof...(Dimensions)] = {};

  constexpr explicit Key(std::size_t index_) noexcept : index(index_) {
    std::size_t key                    = index_;
    constexpr std::size_t dimensions[] = {Dimensions...};
    for (std::size_t idx = 0; idx < size; ++idx) {
      subindices[idx] = key % dimensions[idx];
      key /= dimensions[idx];
    }
  }

  constexpr explicit Key(std::convertible_to<std::size_t> auto... subindices_) noexcept
    requires(sizeof...(subindices_) > 1)
      : index(0)
      , subindices(subindices_...) {
    static_assert(sizeof...(subindices_) == size,
                  "Number of indices must match the number of dimensions.");
    for (std::size_t idx = 0; idx < size; ++idx) {
      index += subindices[idx] * offsets[idx];
    }
  }

  constexpr explicit operator std::size_t() const noexcept { return index; }
  constexpr std::size_t operator[](std::size_t position) const noexcept {
    return subindices[position];
  }
  friend constexpr auto operator<=>(Key const& self, std::size_t other) {
    return self.index <=> other;
  }
  friend constexpr bool operator==(Key const& self, std::size_t other) {
    return self.index == other;
  }
};

template <typename... Variants>  // possibly cv qualified
struct VisitImpl {
  using key_type = Key<variant_size<std::remove_cvref_t<Variants>>::value...>;
  static constexpr std::size_t max_index = key_type::max_index;

  template <key_type Tag, typename = std::make_index_sequence<sizeof...(Variants)>>
  struct Dispatch;

  template <key_type Tag, std::size_t... Idx>
  struct Dispatch<Tag, std::index_sequence<Idx...>> {
    template <typename F, typename... U>
    [[gnu::always_inline]]
    constexpr static decltype(auto) visit(F&& visitor, U&&... variants) {
      return std::forward<F>(visitor)(std::forward<U>(variants).template get_alt<Tag[Idx]>()...);
    }
  };

  template <std::size_t Idx, typename F, typename... U>
  [[gnu::always_inline]]
  constexpr static decltype(auto) visit(F&& visitor, U&&... variants) {
    return Dispatch<key_type{Idx}>::visit(std::forward<F>(visitor), std::forward<U>(variants)...);
  }
};

template <typename Variant>
struct VisitImpl<Variant> {
  using key_type                         = std::size_t;
  static constexpr std::size_t max_index = variant_size<std::remove_cvref_t<Variant>>::value;

  template <std::size_t Idx, typename F, typename U>
  [[gnu::always_inline]] constexpr static decltype(auto) visit(F&& visitor, U&& variant) {
    return std::forward<F>(visitor)(std::forward<U>(variant).template get_alt<Idx>());
  }
};

template <std::size_t Idx,
          typename V,
          typename Alt = typename rsl::variant_alternative<0, std::remove_reference_t<V>>::type>
// TODO: Vs... might be derived from variant
using get_t = std::conditional_t<std::is_lvalue_reference_v<V>, Alt&, Alt&&>;

// non-exhaustive result type check
template <typename F, typename... Vs>
using visit_result_t = std::invoke_result_t<F, get_t<0, Vs>...>;

template <typename R, typename F, typename V>
constexpr R visit_at_enumerated(std::size_t idx, F&& visitor, V&& variant) {
  // This only makes sense for single-variant visitation
  constexpr static std::size_t max_index = rsl::variant_size<std::remove_cvref_t<V>>::value;
  template for (constexpr size_t Idx : std::views::iota(0ZU, max_index)) {
    if (idx == Idx) {
      return std::invoke(std::forward<F>(visitor),
                         std::in_place_index<Idx>,
                         std::forward<V>(variant).template get<Idx>());
    }
  }
  _variant_impl::throw_bad_variant_access(variant.valueless_by_exception());
}

template <typename R, typename F, typename... Vs>
constexpr R visit_at(std::size_t idx, F&& visitor, Vs&&... variants) {
  constexpr static std::size_t max_index = VisitImpl<Vs...>::max_index;
  template for (constexpr size_t Idx : std::views::iota(0ZU, max_index)) {
    if (idx == Idx) {
      return VisitImpl<Vs...>::template visit<Idx>(std::forward<F>(visitor),
                                                   std::forward<Vs>(variants)...);
    }
  }
  _variant_impl::throw_bad_variant_access((variants.valueless_by_exception() || ...));
}
}  // namespace _visit_impl

template <typename R, typename F, typename... Vs>
constexpr R visit(F&& visitor, Vs&&... variants) {
  if constexpr (sizeof...(Vs) == 0) {
    return std::forward<F>(visitor)();
  } else {
    auto const key = typename _visit_impl::VisitImpl<Vs...>::key_type(variants.index()...);
    return _visit_impl::visit_at<R>(std::size_t{key},
                                    std::forward<F>(visitor),
                                    std::forward<Vs>(variants)...);
  }
}

template <typename F, typename... Vs>
constexpr decltype(auto) visit(F&& visitor, Vs&&... variants) {
  using return_type = _visit_impl::visit_result_t<F, Vs...>;
  return rsl::visit<return_type>(std::forward<F>(visitor), std::forward<Vs>(variants)...);
}

namespace _variant_impl {
template <typename Storage>
class variant_base {
protected:
  friend struct variant_alternative;
  friend struct variant_size<variant_base>;

  constexpr void reset() {
    if (discriminator != npos) {
      _visit_impl::visit_at<void>(
          discriminator,
          [](auto&& member) { std::destroy_at(std::addressof(member)); },
          *this);
      discriminator = npos;
    }
  }

  [[nodiscard]] constexpr bool can_nothrow_move() const {
    if (valueless_by_exception()) {
      return true;
    }

    template for (constexpr auto Idx : std::views::iota(0ZU, alternatives.count)) {
      if constexpr (std::is_nothrow_move_constructible_v<[:alternatives.types[Idx]:]>) {
        return true;
      }
    }
    return false;
  }

  template <typename V>
  void do_construct(variant_base& lhs, V&& rhs) {
    lhs.reset();
    if (rhs.valueless_by_exception()) {
      return;
    }

    auto rhs_index = rhs.index();
    _visit_impl::visit_at_enumerated<void>(
        rhs_index,
        [&]<typename T, std::size_t Idx>(std::in_place_index_t<Idx> idx, T&& rhs_alternative) {
          std::construct_at(&lhs, idx, std::forward<T>(rhs_alternative));
        },
        std::forward<V>(rhs));
    lhs.discriminator = index_type(rhs_index);
  }

public:
  constexpr static auto alternatives = [:_impl::cache_members(
                                             nonstatic_data_members_of(
                                                 dealias(^^Storage),
                                                 std::meta::access_context::unchecked()) |
                                             std::views::drop(1)):];
  using index_type = std::conditional_t<(alternatives.count >= 255), unsigned short, unsigned char>;
  static constexpr auto npos = index_type(-1ULL);

protected:
  union {
    Storage storage;
  };
  index_type discriminator = npos;

public:
  // default constructor, only if alternative #0 is default constructible
  constexpr variant_base()                                                    //
      noexcept(is_nothrow_default_constructible_type(alternatives.types[0]))  // [variant.ctor]/5
    requires(is_default_constructible_type(alternatives.types[0]))            // [variant.ctor]/2
      : variant_base(std::in_place_index<0>) {}                               // [variant.ctor]/3

  constexpr variant_base(variant_base const& other) = default;
  constexpr variant_base(variant_base const& other)                              //
      noexcept(std::ranges::all_of(alternatives.types,                           //[variant.ctor]/8
                                   std::meta::is_nothrow_copy_assignable_type))  //
    requires(!std::is_trivially_destructible_v<Storage> &&                       // [variant.ctor]/9
             std::ranges::all_of(alternatives.types,                             //
                                 std::meta::is_copy_constructible_type))         //
  {
    do_construct(*this, other);
  }

  constexpr variant_base(variant_base&& other) = default;
  constexpr variant_base(variant_base&& other)  //
      noexcept(
          std::ranges::all_of(alternatives.types,
                              std::meta::is_nothrow_move_constructible_type))  // [variant.ctor]/12
    requires(!std::is_trivially_destructible_v<Storage> &&                     // [variant.ctor]/13
             std::ranges::all_of(alternatives.types, std::meta::is_move_constructible_type))
  {
    do_construct(*this, std::move(other));
  }

  // TODO rewrite selected_index
  template <typename T>
  constexpr static auto selected_index =
      _variant_impl::selected_index<T, [:substitute(^^_impl::TypeList, alternatives.types):]>;

  // converting constructor
  template <typename T>
    requires(alternatives.count != 0                                 // [variant.ctor]/15.1
             && !std::same_as<std::remove_cvref_t<T>, variant_base>  // [variant.ctor]/15.2
             && !_variant_impl::is_in_place<std::remove_cvref_t<T>>  // [variant.ctor]/15.3
             && selected_index<T> != variant_npos)  // [variant.ctor]/15.4, [variant.ctor]/15.5
  constexpr explicit variant_base(T&& obj)          //
      noexcept(is_nothrow_constructible_type(alternatives.types[selected_index<T>],
                                             {^^T}))  // [variant.ctor]/18
      : variant_base(std::in_place_index<selected_index<T>>, std::forward<T>(obj)) {}

  // in place constructors
  template <typename T, typename... Args>
  constexpr explicit variant_base(std::in_place_type_t<T>, Args&&... args)  //
      noexcept(std::is_nothrow_constructible_v<T, Args...>)                 // [variant.ctor]/23
      : variant_base(std::in_place_index<selected_index<T>>, std::forward<Args>(args)...) {}

  template <typename T, typename U, typename... Args>
  constexpr explicit variant_base(std::in_place_type_t<T>,
                                  std::initializer_list<U> init_list,
                                  Args&&... args)  //
      noexcept(std::is_nothrow_constructible_v<T,
                                               std::initializer_list<U>&,
                                               Args...>)  // [variant.ctor]/28
      : variant_base(std::in_place_index<selected_index<T>>,
                     init_list,
                     std::forward<Args>(args)...) {}

  template <std::size_t Idx, typename... Args>
  constexpr explicit variant_base(std::in_place_index_t<Idx>, Args&&... args)  //
      noexcept(is_nothrow_constructible_type(alternatives.types[Idx], {^^Args...}))
      : discriminator(Idx) {
    // Primary constructor

    std::construct_at(&storage, '\0');
    std::construct_at(alternatives.template get_addr<Idx>(storage), std::forward<Args>(args)...);
  }

  template <std::size_t Idx, typename U, typename... Args>
  constexpr explicit variant_base(std::in_place_index_t<Idx>,
                                  std::initializer_list<U> init_list,
                                  Args&&... args)  //
      noexcept(std::is_nothrow_constructible_v<    //
               [:alternatives.types[Idx]:], std::initializer_list<U>&, Args...>)
      : discriminator(Idx) {
    // Primary constructor

    std::construct_at(&storage, '\0');
    std::construct_at(alternatives.template get_addr<Idx>(storage),
                      init_list,
                      std::forward<Args>(args)...);
  }

  constexpr variant_base& operator=(variant_base const& other) = default;
  constexpr variant_base& operator=(variant_base&& other)      = default;

  constexpr variant_base& operator=(variant_base const& other)
    requires(!std::is_trivially_destructible_v<Storage> &&
             std::ranges::all_of(alternatives.types, std::meta::is_copy_assignable_type))
  {
    if (this != std::addressof(other)) {
      rsl::visit<void>([this]<typename T>(T const& obj) { this->template emplace<T>(obj); }, other);
    }
    return *this;
  }

  constexpr variant_base& operator=(variant_base&& other) noexcept
    requires(!std::is_trivially_destructible_v<Storage> &&
             std::ranges::all_of(alternatives.types, std::meta::is_move_assignable_type))
  {
    if (this != std::addressof(other)) {
      rsl::visit<void>(
          [this]<typename T>(T&& obj) { this->template emplace<T>(std::forward<T>(obj)); },
          std::move(other));
    }
    return *this;
  }

  constexpr ~variant_base()
    requires std::is_trivially_destructible_v<Storage>
  = default;
  constexpr ~variant_base() { reset(); }
  [[nodiscard]] constexpr bool valueless_by_exception() const noexcept {
    return discriminator == npos;
  }
  [[nodiscard]] constexpr std::size_t index() const noexcept {
    if (discriminator != npos) {
      return discriminator;
    }
    return variant_npos;
  }

  template <std::size_t Idx, typename... Args>
  constexpr void emplace(Args&&... args) {
    static_assert(Idx < alternatives.count, "Alternative index out of bounds");

    reset();
    std::construct_at(alternatives.template get_addr<Idx>(storage), std::forward<Args>(args)...);
    discriminator = Idx;
  }

  template <typename T, typename... Args>
  constexpr void emplace(Args&&... args) {
    // TODO is remove_reference required here?
    emplace<alternatives.get_index_of(remove_reference(^^T))>(std::forward<Args>(args)...);
  }

  template <std::size_t Idx, typename Self>
  constexpr decltype(auto) get_alt(this Self&& self) {
    static_assert(Idx < alternatives.count, "Alternative index out of bounds");
    return alternatives.template get<Idx>(std::forward<Self>(self).storage);
  }

  template <std::size_t Idx, typename Self>
  constexpr decltype(auto) get(this Self&& self) {
    if (self.index() != Idx) [[unlikely]] {
      _variant_impl::throw_bad_variant_access(self.valueless_by_exception());
    }

    static_assert(Idx < alternatives.count, "Alternative index out of bounds");
    return std::forward<Self>(self).template get_alt<Idx>();
  }

  template <typename T, typename Self>
  constexpr decltype(auto) get(this Self&& self) {
    // TODO is remove_reference required here?
    return std::forward<Self>(self).template get<alternatives.get_index_of(remove_reference(^^T))>();
  }

  void swap(variant_base& other) {
    if (valueless_by_exception() && other.valueless_by_exception()) {
      // both variants valueless - do nothing
      return;
    }

    auto lhs = this;
    auto rhs = std::addressof(other);
    if (can_nothrow_move() && !other.can_nothrow_move()) {
      std::swap(lhs, rhs);
    }

    variant_base tmp(std::move(other));
    if constexpr (std::ranges::all_of(alternatives.types,
                                      std::meta::is_nothrow_move_constructible_type)) {
      do_construct(*rhs, std::move(*lhs));
    } else {
      // non-standard libc++ extension
      // try moving tmp back into other if move construction from *this fails
      try {
        do_construct(*rhs, std::move(*lhs));
      } catch (...) {
        if (tmp.can_nothrow_move()) {
          do_construct(*rhs, std::move(tmp));
        }
        throw;
      }
    }
    do_construct(*lhs, std::move(tmp));
  }
};

template <typename Storage>
concept is_threeway_comparable =
    std::ranges::all_of(variant_base<Storage>::alternatives.types, [](std::meta::info R) {
      return extract<bool>(substitute(^^std::three_way_comparable, {R}));
    });

consteval std::meta::info common_comparison_category(auto&& R) {
  std::vector<std::meta::info> args;
  for (auto refl : R) {
    args.push_back(substitute(^^std::compare_three_way_result_t, {refl}));
  }
  return substitute(^^std::common_comparison_category_t, args);
}

template <typename Storage>
using common_comparison_category_t = [:common_comparison_category(
                                           variant_base<Storage>::alternatives.types):];
}  // namespace _variant_impl

// needed for visitation in base
template <typename Storage>
struct variant_size<_variant_impl::variant_base<Storage>>
    : std::integral_constant<std::size_t,
                             _variant_impl::variant_base<Storage>::alternatives.count> {};

template <typename Storage>
struct variant_size<_variant_impl::variant_base<Storage> const>
    : variant_size<_variant_impl::variant_base<Storage>> {};

template <std::size_t Idx, typename Storage>
struct variant_alternative<Idx, _variant_impl::variant_base<Storage>> {
  static_assert(Idx < _variant_impl::variant_base<Storage>::alternatives.count,
                "variant_alternative index out of range");
  using type = [:_variant_impl::variant_base<Storage>::alternatives.types[Idx]:];
};

template <std::size_t Idx, typename Storage>
struct variant_alternative<Idx, _variant_impl::variant_base<Storage> const>
    : variant_alternative<Idx, _variant_impl::variant_base<Storage>> {};

template <std::size_t Idx, typename V>
using variant_alternative_t = typename variant_alternative<Idx, std::remove_reference_t<V>>::type;

template <typename T>
inline constexpr std::size_t variant_size_v = rsl::variant_size<T>::value;

struct monostate {};

template <typename Storage>
constexpr auto swap(_variant_impl::variant_base<Storage>& lhs,
                    _variant_impl::variant_base<Storage>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> decltype(lhs.swap(rhs)) {
  return lhs.swap(rhs);
}

template <typename Storage>
  requires(_variant_impl::is_threeway_comparable<Storage>)
constexpr auto operator<=>(_variant_impl::variant_base<Storage> const& lhs,
                           _variant_impl::variant_base<Storage> const& rhs)
    -> _variant_impl::common_comparison_category_t<Storage> {
  using comparison_result = _variant_impl::common_comparison_category_t<Storage>;

  if (lhs.valueless_by_exception() && rhs.valueless_by_exception()) {
    return std::strong_ordering::equal;
  }

  if (lhs.valueless_by_exception()) {
    return std::strong_ordering::less;
  }

  if (rhs.valueless_by_exception()) {
    return std::strong_ordering::greater;
  }

  if (auto index_comparison = lhs.index() <=> rhs.index(); index_comparison != 0) {
    // different alternatives active
    return index_comparison;
  }

  return rsl::visit<comparison_result>(
      []<typename T>(T const& lhs_value, T const& rhs_value) -> comparison_result {
        // compare values
        return lhs_value <=> rhs_value;
      },
      lhs,
      rhs);
}

template <typename Storage>
constexpr bool operator==(_variant_impl::variant_base<Storage> const& lhs,
                          _variant_impl::variant_base<Storage> const& rhs) {
  if (lhs.index() != rhs.index()) {
    // different alternatives active
    return false;
  }
  if (lhs.valueless_by_exception()) {
    // both are valueless
    return true;
  }

  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::equal_to<>>{}, lhs, rhs);
}

template <typename Storage>
constexpr bool operator!=(_variant_impl::variant_base<Storage> const& lhs,
                          _variant_impl::variant_base<Storage> const& rhs) {
  if (lhs.index() != rhs.index()) {
    // different alternatives active
    return true;
  }
  if (lhs.valueless_by_exception()) {
    // both are valueless
    return false;
  }

  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::not_equal_to<>>{}, lhs, rhs);
}

template <typename Storage>
constexpr bool operator<(_variant_impl::variant_base<Storage> const& lhs,
                         _variant_impl::variant_base<Storage> const& rhs) {
  if (rhs.valueless_by_exception()) {
    return false;
  }
  if (lhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return false;
  }
  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::less<>>{}, lhs, rhs);
}

template <typename Storage>
constexpr bool operator>(_variant_impl::variant_base<Storage> const& lhs,
                         _variant_impl::variant_base<Storage> const& rhs) {
  if (lhs.valueless_by_exception()) {
    return false;
  }
  if (rhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return false;
  }
  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::greater<>>{}, lhs, rhs);
}

template <typename Storage>
constexpr bool operator<=(_variant_impl::variant_base<Storage> const& lhs,
                          _variant_impl::variant_base<Storage> const& rhs) {
  if (lhs.valueless_by_exception()) {
    return false;
  }
  if (rhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return false;
  }
  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::less_equal<>>{}, lhs, rhs);
}

template <typename Storage>
constexpr bool operator>=(_variant_impl::variant_base<Storage> const& lhs,
                          _variant_impl::variant_base<Storage> const& rhs) {
  if (rhs.valueless_by_exception()) {
    return false;
  }
  if (lhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return false;
  }
  return rsl::visit<bool>(_variant_impl::ComparisonVisitor<std::greater_equal<>>{}, lhs, rhs);
}

constexpr bool operator==(monostate, monostate) noexcept {
  return true;
}
constexpr std::strong_ordering operator<=>(monostate, monostate) noexcept {
  return std::strong_ordering::equal;
}

//? [variant.get], value access

/**
 * @brief Check if the desired alternative is currently held
 * @warning non-standard extension
 * @tparam Idx index of the desired alternative
 * @tparam Ts
 * @param obj
 * @return true
 * @return false
 */
template <std::size_t Idx, typename Storage>
constexpr bool holds_alternative(_variant_impl::variant_base<Storage> const& obj) noexcept {
  return obj.index() == Idx;
}

/**
 * @brief Check if the desired alternative is currently held
 * @warning non-standard extension
 * @tparam T type of the desired alternative
 * @tparam Ts
 * @param obj
 * @return true
 * @return false
 */
template <class T, typename Storage>
constexpr bool holds_alternative(_variant_impl::variant_base<Storage> const& obj) noexcept {
  return obj.index() == obj.alternatives.get_index_of(^^T);
}

template <std::size_t Idx, _variant_impl::has_get V>
constexpr decltype(auto) get(V&& variant_) {
  if (variant_.index() != Idx) [[unlikely]] {
    _variant_impl::throw_bad_variant_access(variant_.valueless_by_exception());
  }
  return std::forward<V>(variant_).template get<Idx>();
}

template <typename T, _variant_impl::has_get V>
constexpr decltype(auto) get(V&& variant_) {
  return rsl::get<T>(std::forward<V>(variant_));
}

template <std::size_t Idx, typename Storage>
constexpr auto get_if(_variant_impl::variant_base<Storage>* variant_) noexcept
    -> variant_alternative_t<Idx, _variant_impl::variant_base<Storage>>* {
  constexpr static auto alt_count = _variant_impl::variant_base<Storage>::alternatives.count;
  static_assert(Idx < alt_count,
                std::string("index must be in [0, ") + _impl::to_string(alt_count) + "]");
  static_assert(!std::is_void_v<variant_alternative_t<Idx, _variant_impl::variant_base<Storage>>>,
                "alternative type must not be void");

  if (variant_ && variant_->index() == Idx) {
    return std::addressof((*variant_).template get_alt<Idx>());
  }
  return nullptr;
}

template <typename T, typename Storage>
constexpr auto* get_if(_variant_impl::variant_base<Storage>* variant_) noexcept {
  constexpr static std::size_t index = variant_->alternatives.get_index_of(^^T);
  static_assert(index < variant_->alternatives.count, "T must occur exactly once in alternatives");
  return rsl::get_if<index>(variant_);
}
}  // namespace rsl