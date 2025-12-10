#include <rsl/enum>
#include <gtest/gtest.h>

namespace {}

TEST(NumericLimits, FixedEnum) {
  enum T1 : int { A1 = -1 };
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T1));
  ASSERT_EQ(rsl::numeric_limits<T1>::min(), std::numeric_limits<int>::min());
  ASSERT_EQ(rsl::numeric_limits<T1>::max(), std::numeric_limits<int>::max());

  enum class T2 { A2 = -1 };
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T2));
  ASSERT_EQ(rsl::numeric_limits<T2>::min(), std::numeric_limits<int>::min());
  ASSERT_EQ(rsl::numeric_limits<T2>::max(), std::numeric_limits<int>::max());

  enum class T3 {};
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T3));
  ASSERT_EQ(rsl::numeric_limits<T3>::min(), std::numeric_limits<int>::min());
  ASSERT_EQ(rsl::numeric_limits<T3>::max(), std::numeric_limits<int>::max());

  enum T4 : int {};
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T4));
  ASSERT_EQ(rsl::numeric_limits<T4>::min(), std::numeric_limits<int>::min());
  ASSERT_EQ(rsl::numeric_limits<T4>::max(), std::numeric_limits<int>::max());

  enum T5 : short {};
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T5));
  ASSERT_EQ(rsl::numeric_limits<T5>::min(), std::numeric_limits<short>::min());
  ASSERT_EQ(rsl::numeric_limits<T5>::max(), std::numeric_limits<short>::max());

  enum class T6 : short {};
  ASSERT_TRUE(rsl::has_fixed_underlying_type(^^T6));
  ASSERT_EQ(rsl::numeric_limits<T6>::min(), std::numeric_limits<short>::min());
  ASSERT_EQ(rsl::numeric_limits<T6>::max(), std::numeric_limits<short>::max());
}

TEST(NumericLimits, MinWidthEnum) {
  enum T1 {};
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T1));
  ASSERT_EQ(rsl::numeric_limits<T1>::min(), 0);
  ASSERT_EQ(rsl::numeric_limits<T1>::max(), 1);

  enum T2 { A1 = 5 };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T2));
  ASSERT_EQ(rsl::numeric_limits<T2>::min(), 0);
  ASSERT_EQ(rsl::numeric_limits<T2>::max(), 7);

  enum T3 { A = -1, B = 1 };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T3));
  ASSERT_EQ(rsl::numeric_limits<T3>::min(), -2);
  ASSERT_EQ(rsl::numeric_limits<T3>::max(), 1);

  enum T4 { A4 = -5 };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T4));
  ASSERT_EQ(rsl::numeric_limits<T4>::min(), -8);
  ASSERT_EQ(rsl::numeric_limits<T4>::max(), 7);

  enum T5 { A5 = -9223372036854775807LL };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T5));
  ASSERT_EQ(rsl::numeric_limits<T5>::min(), -9223372036854775807LL - 1);
  ASSERT_EQ(rsl::numeric_limits<T5>::max(), 9223372036854775807LL);

  enum T6 { A6 = -1 };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T6));
  ASSERT_EQ(rsl::numeric_limits<T6>::min(), -1);
  ASSERT_EQ(rsl::numeric_limits<T6>::max(), 0);

  enum T7 { A7 = -1, B7 = -5 };
  ASSERT_FALSE(rsl::has_fixed_underlying_type(^^T7));
  ASSERT_EQ(rsl::numeric_limits<T7>::min(), -8);
  ASSERT_EQ(rsl::numeric_limits<T7>::max(), 7);
}