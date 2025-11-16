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

}
