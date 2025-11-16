#include <rsl/enum>
#include <gtest/gtest.h>

namespace {
enum class[[= rsl::flag_enum]] Permissions : uint8_t {
  None    = 0U,
  Read    = 1U << 0U,
  Write   = 1U << 1U,
  Execute = 1U << 2U,
  All     = Read | Write | Execute
};
}

TEST(FlagEnum, HasFlag) {
  ASSERT_TRUE(rsl::has_flag(Permissions::All, Permissions::Read));
  ASSERT_TRUE(rsl::has_flag(Permissions::All, Permissions::All));

  ASSERT_TRUE(rsl::has_flag(Permissions::All, 1));
  ASSERT_TRUE(rsl::has_flag(Permissions::All, 0b11));
}

TEST(FlagEnum, Operators) {
  // operator |
  ASSERT_EQ(static_cast<uint8_t>(Permissions::Read | Permissions::Write), 0b11);
  ASSERT_EQ(static_cast<uint8_t>(1 | Permissions::Write), 0b11);
  ASSERT_EQ(static_cast<uint8_t>(Permissions::Write | 1), 0b11);

  // operator &
  ASSERT_TRUE(static_cast<uint8_t>(Permissions::All & Permissions::Read) != 0);
  ASSERT_TRUE(static_cast<uint8_t>(Permissions::All & 1) != 0);
  ASSERT_TRUE(static_cast<uint8_t>(0b111 & Permissions::Read) != 0);

  // operator ^
  ASSERT_EQ(static_cast<uint8_t>(Permissions::All ^ Permissions::Read), 0b110);
  ASSERT_EQ(static_cast<uint8_t>(Permissions::All ^ 1), 0b110);
  ASSERT_EQ(static_cast<uint8_t>(0b111 ^ Permissions::Read), 0b110);

  // operator ==
  ASSERT_TRUE(Permissions::Read == static_cast<uint8_t>(1));
  ASSERT_TRUE(static_cast<uint8_t>(1) == Permissions::Read);

  // operator !=
  ASSERT_TRUE(Permissions::Read != static_cast<uint8_t>(2));
  ASSERT_TRUE(static_cast<uint8_t>(2) != Permissions::Read);
}

TEST(FlagEnum, CompoundOperator) {
  // operator|=
  {
    Permissions p1 = Permissions::None;
    Permissions p2 = Permissions::None;
    uint8_t p3     = 0;

    p1 |= Permissions::Read;
    p2 |= 1;
    p3 |= Permissions::Read;

    ASSERT_EQ(p1, Permissions::Read);
    ASSERT_EQ(p2, Permissions::Read);
    ASSERT_EQ(p3, 1);
  }

  // operator &=
  {
    Permissions p1 = Permissions::All;
    Permissions p2 = Permissions::All;
    uint8_t p3     = 0b111;

    p1 &= Permissions::Write;
    p2 &= 1U << 1U;
    p3 &= Permissions::Write;

    ASSERT_EQ(static_cast<uint8_t>(p1), 0b10);
    ASSERT_EQ(static_cast<uint8_t>(p2), 0b10);
    ASSERT_EQ(static_cast<uint8_t>(p3), 0b10);
  }

  // operator ^=
  {
    Permissions p1 = Permissions::All;
    Permissions p2 = Permissions::All;
    uint8_t p3     = 0b111;

    p1 ^= Permissions::Execute;
    p2 ^= 1U << 2U;
    p3 ^= Permissions::Execute;

    ASSERT_EQ(static_cast<uint8_t>(p1), 0b11);
    ASSERT_EQ(static_cast<uint8_t>(p2), 0b11);
    ASSERT_EQ(static_cast<uint8_t>(p3), 0b11);
  }
}