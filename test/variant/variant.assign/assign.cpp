#include <gtest/gtest.h>
#include <rsl/variant>
#include <rsl/string_view>

#include <common/assertions.h>

TEST(Assign, NonConverting) {
  {
    rsl::variant<int, float> v(43);
    v = 42;
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(rsl::get<0>(v), 42);
  }
  {
    rsl::variant<int, float> v(4.3f);
    ASSERT_EQ(v.index(), 1);
    ASSERT_EQ(rsl::get<1>(v), 4.3f);
    v = 42;
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(rsl::get<0>(v), 42);
  }
  {
    rsl::variant<unsigned, long> v;
    v = 42;
    ASSERT_EQ(v.index(), 1);
    ASSERT_EQ(rsl::get<1>(v), 42);
    v = 43u;
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(rsl::get<0>(v), 43);
  }
  {
    rsl::variant<rsl::string_view, int> v(3);
    v = "bar";
    assert(v.index() == 0);
    assert(rsl::get<0>(v) == "bar");
  }
}
