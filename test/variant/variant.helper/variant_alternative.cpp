#include <gtest/gtest.h>
#include <cstddef>

#include <rsl/variant>
#include <common/assertions.h>

template <class Variant, std::size_t Idx, class Expected>
void check_alternatives() {
  // type
  ASSERT_SAME((typename rsl::variant_alternative<Idx, Variant>::type), Expected);
  ASSERT_SAME((typename rsl::variant_alternative<Idx, const Variant>::type), const Expected);

  // alias
  ASSERT_SAME((rsl::variant_alternative_t<Idx, Variant>), Expected);
  ASSERT_SAME((rsl::variant_alternative_t<Idx, const Variant>), const Expected);
}

union AlternativeUnion {
  int member_0;
  void* member_1;
  const void* member_2;
  long double member_3;
};

constexpr void test_alternative() {
  using variant_t = rsl::variant<int, void*, const void*, long double>;
  static_assert(std::same_as<typename rsl::variant_alternative<0, variant_t>::type, int>);
  static_assert(std::same_as<typename rsl::variant_alternative<1, variant_t>::type, void*>);
  static_assert(std::same_as<typename rsl::variant_alternative<2, variant_t>::type, const void*>);
  static_assert(std::same_as<typename rsl::variant_alternative<3, variant_t>::type, long double>);

  static_assert(std::same_as<typename rsl::variant_alternative<0, const variant_t>::type, int const>);
  static_assert(std::same_as<typename rsl::variant_alternative<1, const variant_t>::type, void* const>);
  static_assert(std::same_as<typename rsl::variant_alternative<2, const variant_t>::type, const void* const>);
  static_assert(std::same_as<typename rsl::variant_alternative<3, const variant_t>::type, long double const>);

  
  static_assert(std::same_as<rsl::variant_alternative_t<0, variant_t>, int>);
  static_assert(std::same_as<rsl::variant_alternative_t<1, variant_t>, void*>);
  static_assert(std::same_as<rsl::variant_alternative_t<2, variant_t>, const void*>);
  static_assert(std::same_as<rsl::variant_alternative_t<3, variant_t>, long double>);

  static_assert(std::same_as<rsl::variant_alternative_t<0, const variant_t>, int const>);
  static_assert(std::same_as<rsl::variant_alternative_t<1, const variant_t>, void* const>);
  static_assert(std::same_as<rsl::variant_alternative_t<2, const variant_t>, const void* const>);
  static_assert(std::same_as<rsl::variant_alternative_t<3, const variant_t>, long double const>);
}

// todo ensure out of bounds access causes a static assertion failure