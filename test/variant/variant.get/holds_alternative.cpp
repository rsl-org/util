#include <gtest/gtest.h>

#include <rsl/variant>
#include <common/assertions.h>

TEST(Helpers, HoldsAlternative) {
  constexpr rsl::variant<short, long> variant;
  ASSERT_TRUE(rsl::holds_alternative<short>(variant));
  ASSERT_TRUE(!rsl::holds_alternative<long>(variant));
}

TEST(Helpers, HoldsAlternativeNoexcept){
  const rsl::variant<int> variant;
  ASSERT_NOEXCEPT(rsl::holds_alternative<int>(variant));
}