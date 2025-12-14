#include <gtest/gtest.h>
#include <rsl/variant>
#include <rsl/string_view>

#include <common/assertions.h>

const rsl::string_view& get_const_ref_string_view() {
  static rsl::string_view sv("foo");
  return sv;
}

TEST(Assign, NonConverting) {
  {
    rsl::variant<int, float> v(43);
    v = 42;
    ASSERT_EQ(v.index(), 0);
    ASSERT_SAME(decltype(get<0>(v)), int&);
    ASSERT_EQ(get<0>(v), 42);
  }
  {
    rsl::variant<int, float> v(43);
    const int i = 42;
    v           = i;
    ASSERT_EQ(v.index(), 0);
    ASSERT_SAME(decltype(get<0>(v)), int&);
    ASSERT_EQ(get<0>(v), 42);
  }
  {
    rsl::variant<rsl::string_view, float> v(43.0f);
    v = get_const_ref_string_view();
    ASSERT_SAME(decltype(get<0>(v)), rsl::string_view&);
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(get<0>(v), rsl::string_view{"foo"});
  }
  {
    rsl::variant<int, float> v(4.3f);
    ASSERT_EQ(v.index(), 1);
    ASSERT_EQ(get<1>(v), 4.3f);
    v = 42;
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(get<0>(v), 42);
  }
  {
    rsl::variant<unsigned, long> v;
    v = 42;
    ASSERT_EQ(v.index(), 1);
    ASSERT_EQ(get<1>(v), 42);
    v = 43u;
    ASSERT_EQ(v.index(), 0);
    ASSERT_EQ(get<0>(v), 43);
  }
  {
    rsl::variant<rsl::string_view, int> v(3);
    v = "bar";
    ASSERT_EQ(v.index() , 0);
    ASSERT_EQ(get<0>(v) , "bar");
  }
}
