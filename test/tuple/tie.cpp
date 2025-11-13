#include <rsl/tuple>
#include <common/assertions.h>
#include <gtest/gtest.h>

TEST(Tuple, Tie) {
  int i         = 42;
  double f      = 1.1;
  using ExpectT = rsl::tuple<int&, decltype(std::ignore)&, double&>;
  auto res      = rsl::tie(i, std::ignore, f);
  static_assert(std::is_same<ExpectT, decltype(res)>::value, "");
  ASSERT_TRUE(&rsl::get<0>(res) == &i);
  ASSERT_TRUE(&rsl::get<1>(res) == &std::ignore);
  ASSERT_TRUE(&rsl::get<2>(res) == &f);

  // TODO: make this work
  // res = rsl::make_tuple(101, nullptr, -1.0);
  // ASSERT_TRUE(i == 101);
  // ASSERT_TRUE(f == -1.0);
}

TEST(tuple, TieAssign) {
  int i = 0;
  std::string s;

  // TODO: make this work
  // rsl::tie(i, std::ignore, s) = rsl::make_tuple(42, 3.14, "C++");
  // ASSERT_TRUE(i == 42);
  // ASSERT_TRUE(s == "C++");
}
