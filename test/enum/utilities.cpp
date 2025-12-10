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
  ASSERT_TRUE(rsl::is_named_enumerator<Foo>(Foo::bar));
  ASSERT_TRUE(rsl::is_named_enumerator<Foo>(Foo::foo));
  ASSERT_FALSE(rsl::is_named_enumerator<Foo>(20));
}

TEST(Enum, IsValid) {
  ASSERT_TRUE(rsl::is_valid_enumerator<Foo>(Foo::foo));
  ASSERT_TRUE(rsl::is_valid_enumerator<Foo>(Foo::bar));
  ASSERT_TRUE(rsl::is_valid_enumerator<Foo>(Foo::foo | Foo::bar));
  ASSERT_TRUE(rsl::is_valid_enumerator<Foo>(15));
  ASSERT_FALSE(rsl::is_valid_enumerator<Foo>(16));

  ASSERT_TRUE(rsl::is_valid_enumerator<Bar>(Bar::foo1));
  ASSERT_TRUE(rsl::is_valid_enumerator<Bar>(Bar::foo2));
  ASSERT_TRUE(rsl::is_valid_enumerator<Bar>(Bar(-8)));
  ASSERT_TRUE(rsl::is_valid_enumerator<Bar>(Bar(7)));
  ASSERT_FALSE(rsl::is_valid_enumerator<Bar>(-9));
  ASSERT_FALSE(rsl::is_valid_enumerator<Bar>(8));

  ASSERT_TRUE(rsl::is_valid_enumerator<Baz>(Baz::foo));
  ASSERT_TRUE(rsl::is_valid_enumerator<Baz>(Baz::bar));
  ASSERT_TRUE(rsl::is_valid_enumerator<Baz>(Baz::foo | Baz::bar));
  ASSERT_TRUE(rsl::is_valid_enumerator<Baz>(15));
  ASSERT_FALSE(rsl::is_valid_enumerator<Baz>(16));

  ASSERT_TRUE(rsl::is_valid_enumerator<Zoinks>(Zoinks::foo));
  ASSERT_TRUE(rsl::is_valid_enumerator<Zoinks>(Zoinks::bar));
  ASSERT_TRUE(rsl::is_valid_enumerator<Zoinks>(1234));
}