#include <type_traits>
#include <string>

#include <gtest/gtest.h>
#include <rsl/variant>
#include "common/util.h"

TEST(ConvertingConstructor, ExactMatch) {
  using test_variant = rsl::variant<Constant<0>, Constant<1>, Constant<2>>;
  ASSERT_TRUE((std::is_constructible_v<test_variant, Constant<1>>));

  ASSERT_EQ((rsl::_variant_impl::selected_index<Constant<1>, rsl::_impl::TypeList<Constant<0>, Constant<1>, Constant<2>>>), 1);
}

TEST(ConvertingConstructor, Convertible) {
  using test_variant = rsl::variant<bool>;
  ASSERT_TRUE((std::is_constructible_v<test_variant, std::true_type>));

  // sanity check
  ASSERT_FALSE((std::is_constructible_v<test_variant, std::string>));
  ASSERT_FALSE((std::is_constructible_v<test_variant, std::nullptr_t>));
}

TEST(ConvertingConstructor, TypePromotion) {
  using test_variant = rsl::variant<long>;
  ASSERT_TRUE((std::is_constructible_v<test_variant, short>));

  // check if it selects the right alternative
  ASSERT_EQ((rsl::_variant_impl::selected_index<short, rsl::_impl::TypeList<char, long>>), 1);
}

TEST(ConvertingConstructor, Invalid) {
  using ambiguous_variant = rsl::variant<int, int>;
  using test_variant = rsl::variant<short, int>;

  // ambiguous
  ASSERT_EQ((rsl::_variant_impl::selected_index<int, rsl::_impl::TypeList<int, int>>), rsl::variant_npos);
  ASSERT_EQ((rsl::_variant_impl::selected_index<int, rsl::_impl::TypeList<long, long long>>), rsl::variant_npos);  
  ASSERT_FALSE((std::is_constructible_v<ambiguous_variant, int>));

  // not contained
  ASSERT_EQ((rsl::_variant_impl::selected_index<float, rsl::_impl::TypeList<short, long>>), rsl::variant_npos);
  ASSERT_FALSE((std::is_constructible_v<test_variant, float>));

  // narrowing
  ASSERT_EQ((rsl::_variant_impl::selected_index<long long, rsl::_impl::TypeList<short, int>>), rsl::variant_npos);
  ASSERT_FALSE((std::is_constructible_v<test_variant, long long>));
}
