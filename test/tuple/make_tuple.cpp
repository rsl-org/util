#include <rsl/tuple>
#include <common/assertions.h>
#include <gtest/gtest.h>

TEST(Tuple, make_tuple) {
  int i                           = 0;
  float j                         = 0;
  rsl::tuple<int, int&, float&> t = rsl::make_tuple(1, std::ref(i), std::ref(j));
  ASSERT_TRUE(rsl::get<0>(t) == 1);
  ASSERT_TRUE(rsl::get<1>(t) == 0);
  ASSERT_TRUE(rsl::get<2>(t) == 0);
  i = 2;
  j = 3.5;
  ASSERT_TRUE(rsl::get<0>(t) == 1);
  ASSERT_TRUE(rsl::get<1>(t) == 2);
  ASSERT_TRUE(rsl::get<2>(t) == 3.5);
  rsl::get<1>(t) = 0;
  rsl::get<2>(t) = 0;
  ASSERT_TRUE(i == 0);
  ASSERT_TRUE(j == 0);

  {
    constexpr auto t1   = rsl::make_tuple(0, 1, 3.14);
    constexpr int i1    = rsl::get<1>(t1);
    constexpr double d1 = rsl::get<2>(t1);
    ASSERT_TRUE(i1 == 1);
    ASSERT_TRUE(d1 == 3.14);
  }
}
