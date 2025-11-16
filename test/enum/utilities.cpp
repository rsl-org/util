#include <rsl/enum>
#include <gtest/gtest.h>

namespace {
enum class Foo : uint8_t {
  foo,
  bar = 10
};
}

TEST(Enum, IsNamed) {
  ASSERT_TRUE(rsl::is_named_enumerator(Foo::bar));
  ASSERT_TRUE(rsl::is_named_enumerator(Foo::foo));
  ASSERT_FALSE(rsl::is_named_enumerator(Foo(20)));
}
