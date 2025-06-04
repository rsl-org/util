#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsl/tagged_variant>
#include <utility>

#include "common/assertions.h"

enum class ScopedEnum {
  INT[[= rsl::type<int>]],
  CHAR[[= rsl::type<char>]],
  BOOL[[= rsl::type<bool>]]
};

namespace unscoped {
enum UnscopedEnum {  //
  INT[[= rsl::type<int>]],
  CHAR[[= rsl::type<char>]],
  BOOL[[= rsl::type<bool>]]
};
}

TEST(TaggedVariant, Switch) {
  using type = rsl::tagged_variant<ScopedEnum>;
  type variant{42};
  switch (variant) {
    using enum type::tags;
    case INT:  ASSERT_EQ(get<INT>(variant), 42); break;
    case CHAR: FAIL();
    case BOOL: FAIL();
  }
  
  variant = type{std::in_place_index<2>, false};
  switch (variant) {
    using enum type::tags;
    case INT:  FAIL();
    case CHAR: FAIL();
    case BOOL: ASSERT_EQ(variant->BOOL, false); break;
  }
}
