#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsl/tagged_variant>

#include "common/assertions.h"

enum class ScopedEnum {
  INT[[= rsl::type<int>]],
  CHAR[[= rsl::type<char>]],
  BOOL[[= rsl::type<bool>]]
};

enum UnscopedEnum { INT[[= rsl::type<int>]], CHAR[[= rsl::type<char>]], BOOL[[= rsl::type<bool>]] };

TEST(TaggedVariant, GenerateType) {
  {
    using type = rsl::variant_for<ScopedEnum>;
    ASSERT_SAME(type, (rsl::variant<int, char, bool>));
  }

  {
    using type = rsl::variant_for<UnscopedEnum>;
    ASSERT_SAME(type, (rsl::variant<int, char, bool>));
  }
}