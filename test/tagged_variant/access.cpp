#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsl/variant>

#include "common/assertions.h"

enum class ScopedEnum {
  INT[[= rsl::type<int>]],
  CHAR[[= rsl::type<char>]],
  BOOL[[= rsl::type<bool>]]
};

enum UnscopedEnum {  //
  INT[[= rsl::type<int>]],
  CHAR[[= rsl::type<char>]],
  BOOL[[= rsl::type<bool>]]
};

TEST(TaggedVariant, GetTag) {
  using type = rsl::tagged_variant<ScopedEnum>;
  type variant{42};
  ASSERT_EQ(get_tag<ScopedEnum>(variant), ScopedEnum::INT);

  variant = type{std::in_place_index<2>, false};
  ASSERT_EQ(get_tag<ScopedEnum>(variant), ScopedEnum::BOOL);
}

TEST(TaggedVariant, Get) {
  using type = rsl::tagged_variant<ScopedEnum>;
  type variant{42};
  ASSERT_EQ(get<ScopedEnum::INT>(variant), 42);

  variant = type{std::in_place_index<2>, false};
  ASSERT_EQ(*get_if<ScopedEnum::BOOL>(&variant), false);
}