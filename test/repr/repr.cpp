#include <gtest/gtest.h>
#include <common/assertions.h>

#include <rsl/repr>

TEST(Repr, Fundamental) {
  ASSERT_EQ(rsl::repr(true), "true");
  ASSERT_EQ(rsl::repr(false), "false");
  ASSERT_EQ(rsl::repr(0), "0");
  ASSERT_EQ(rsl::repr(0ZU), "0UL"); // cannot distinguish from aliased type
  ASSERT_EQ(rsl::repr(0L), "0L");
  ASSERT_EQ(rsl::repr(0LL), "0LL");
  ASSERT_EQ(rsl::repr(0U), "0U");
  ASSERT_EQ(rsl::repr(0UL), "0UL");
  ASSERT_EQ(rsl::repr(0ULL), "0ULL");

  ASSERT_EQ(rsl::repr(1.2), "1.2");
  ASSERT_EQ(rsl::repr(1.2F), "1.2F");
  ASSERT_EQ(rsl::repr(1.2L), "1.2L");

  ASSERT_EQ(rsl::repr("0"), "\"0\"");
  ASSERT_EQ(rsl::repr('0'), "'0'");
}

namespace {
namespace TestNS {
struct TestAgg {
  int a;
  char b;
};
}  // namespace TestNS
}  // namespace

TEST(Repr, Aggregates) {
  auto obj = TestNS::TestAgg{42, 'c'};
  ASSERT_EQ(rsl::repr(obj), "TestAgg{42, 'c'}");
  ASSERT_EQ(rsl::repr(obj, {.names=rsl::NameMode::qualified}), "TestNS::TestAgg{42, 'c'}");
}