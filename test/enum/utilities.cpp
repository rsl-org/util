#include <rsl/enum>
#include <gtest/gtest.h>

namespace {
enum Foo {
  foo,
  bar = 10
};

enum Bar {
  foo1 = -1,
  foo2 = 5
};

enum class [[=rsl::flag_enum]] Baz : int {
  foo,
  bar = 10
};

enum class Zoinks : int {
  foo,
  bar = 10
};
}

TEST(Enum, IsNamed) {
  ASSERT_TRUE(rsl::in_enum<Foo>(Foo::bar));
  ASSERT_TRUE(rsl::in_enum<Foo>(Foo::foo));
  ASSERT_FALSE(rsl::in_enum<Foo>(20));
}

TEST(Enum, InRange) {
  ASSERT_TRUE(rsl::in_range<Foo>(Foo::foo));
  ASSERT_TRUE(rsl::in_range<Foo>(Foo::bar));
  ASSERT_TRUE(rsl::in_range<Foo>(Foo::foo | Foo::bar));
  ASSERT_TRUE(rsl::in_range<Foo>(15));
  ASSERT_FALSE(rsl::in_range<Foo>(16));

  ASSERT_TRUE(rsl::in_range<Bar>(Bar::foo1));
  ASSERT_TRUE(rsl::in_range<Bar>(Bar::foo2));
  ASSERT_TRUE(rsl::in_range<Bar>(Bar(-8)));
  ASSERT_TRUE(rsl::in_range<Bar>(Bar(7)));
  ASSERT_FALSE(rsl::in_range<Bar>(-9));
  ASSERT_FALSE(rsl::in_range<Bar>(8));

  ASSERT_TRUE(rsl::in_range<Baz>(Baz::foo));
  ASSERT_TRUE(rsl::in_range<Baz>(Baz::bar));
  ASSERT_TRUE(rsl::in_range<Baz>(Baz::foo | Baz::bar));
  ASSERT_TRUE(rsl::in_range<Baz>(15));
  ASSERT_FALSE(rsl::in_range<Baz>(16));

  ASSERT_TRUE(rsl::in_range<Zoinks>(Zoinks::foo));
  ASSERT_TRUE(rsl::in_range<Zoinks>(Zoinks::bar));
  ASSERT_TRUE(rsl::in_range<Zoinks>(1234));
}