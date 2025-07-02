#include <string>
#include <gtest/gtest.h>
#include <common/assertions.h>

#include <rsl/expect>

TEST(Expect, Logic) {
  using namespace rsl::placeholders;
  ASSERT_TRUE(expect(_0 > 1)(10));
}

TEST(Expect, ArrayOfChecks) {
  using namespace rsl::placeholders;

  std::function<bool(int)> checks[] = {
    expect(_0 > 1),
    expect(20 < _0)
  };

  for (auto const& check : checks) {
    int x = 50;
    ASSERT_TRUE(check(x));
  }
}