#pragma once
#include <gtest/gtest.h>
#include "type_name.h"

#define DEPAREN(X) ESC(ISH X)
#define ISH(...)   ISH __VA_ARGS__
#define ESC(...)   ESC_(__VA_ARGS__)
#define ESC_(...)  VAN##__VA_ARGS__
#define VANISH

#define ASSERT_SAME(first, second)                                                                                 \
  ASSERT_TRUE((std::is_same_v<DEPAREN(first), DEPAREN(second)>)) << "  Actual: " << nameof<DEPAREN(first)> << '\n' \
                                                                 << "Expected: " << nameof<DEPAREN(second)>

#define ASSERT_STATIC(...) ASSERT_SAME(std::bool_constant<(__VA_ARGS__)>, std::true_type)

#define ASSERT_NOEXCEPT(...) ASSERT_TRUE((noexcept(__VA_ARGS__))) << "Operation must be noexcept"
#define ASSERT_NOT_NOEXCEPT(...) ASSERT_FALSE((noexcept(__VA_ARGS__))) << "Operation must not be noexcept"

#define TEST_T(test_suite_name, test_name, ...)                \
  template <typename>                                          \
  void test_suite_name##_##test_name##_ptest_body();           \
  TEST(test_suite_name, test_name) {                           \
    []<typename... Ts>() {                                     \
      (..., test_suite_name##_##test_name##_ptest_body<Ts>()); \
    }.template operator()<__VA_ARGS__>();                      \
  }                                                            \
  template <typename TypeParam>                                \
  void test_suite_name##_##test_name##_ptest_body()
