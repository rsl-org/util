#include <rsl/serializer/binary/type/deserializer.hpp>
#include <rsl/serializer/binary/type/serializer.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace rsl::serializer::_binary_impl;

// https://docs.google.com/spreadsheets/d/1oCnX0yknu_y4-xb_VE-p8B_RrRUZ5gZ4EQtrTM4KB5o/edit?usp=sharing

TEST(BinaryTypeSerializer, BuiltinsUInt) {
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint8_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint16_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint32_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint64_t, false).size(), 1);

  ASSERT_EQ(TypeSerializer::serialize_type(^^uint8_t, false)[0], 0b1100'0000);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint16_t, false)[0], 0b1100'0001);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint32_t, false)[0], 0b1100'0010);
  ASSERT_EQ(TypeSerializer::serialize_type(^^uint64_t, false)[0], 0b1100'0011);
}

TEST(BinaryTypeSerializer, BuiltinsInt) {
  ASSERT_EQ(TypeSerializer::serialize_type(^^int8_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int16_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int32_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int64_t, false).size(), 1);

  ASSERT_EQ(TypeSerializer::serialize_type(^^int8_t, false)[0], 0b1100'0100);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int16_t, false)[0], 0b1100'0101);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int32_t, false)[0], 0b1100'0110);
  ASSERT_EQ(TypeSerializer::serialize_type(^^int64_t, false)[0], 0b1100'0111);
}

TEST(BinaryTypeSerializer, BuiltinsChar) {
  ASSERT_EQ(TypeSerializer::serialize_type(^^char8_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char16_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char32_t, false).size(), 1);

  ASSERT_EQ(TypeSerializer::serialize_type(^^char8_t, false)[0], 0b1000'0000);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char16_t, false)[0], 0b1000'0001);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char32_t, false)[0], 0b1000'0010);
}

TEST(BinaryTypeSerializer, BuiltinsFloat) {
  ASSERT_EQ(TypeSerializer::serialize_type(^^float, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^double, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^long double, false).size(), 1);

  ASSERT_EQ(TypeSerializer::serialize_type(^^float, false)[0], 0b1000'0100);
  ASSERT_EQ(TypeSerializer::serialize_type(^^double, false)[0], 0b1000'0101);
  ASSERT_EQ(TypeSerializer::serialize_type(^^long double, false)[0], 0b1000'0110);
}

TEST(BinaryTypeSerializer, BuiltinsSpecial) {
  ASSERT_EQ(TypeSerializer::serialize_type(^^bool, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^wchar_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^nullptr_t, false).size(), 1);
  ASSERT_EQ(TypeSerializer::serialize_type(^^void, false).size(), 1);

  ASSERT_EQ(TypeSerializer::serialize_type(^^bool, false)[0], 0b0100'0000);
  ASSERT_EQ(TypeSerializer::serialize_type(^^char, false)[0], 0b0100'0001);
  ASSERT_EQ(TypeSerializer::serialize_type(^^wchar_t, false)[0], 0b0100'0010);
  ASSERT_EQ(TypeSerializer::serialize_type(^^nullptr_t, false)[0], 0b0100'0011);
  ASSERT_EQ(TypeSerializer::serialize_type(^^void, false)[0], 0b0100'0100);
}

namespace {
void test_flags(std::span<uint8_t const> result, std::vector<uint8_t> expected) {
  ASSERT_EQ(result.size(), expected.size());
  for (size_t idx = 0; idx < result.size(); ++idx) {
    SCOPED_TRACE(idx);
    ASSERT_EQ(result[idx], expected[idx]);
  }
}
}  // namespace

TEST(BinaryTypeSerializer, Flags) {
  using ::testing::ElementsAre;
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const), ElementsAre(0b0101'0000, Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool volatile), ElementsAre(0b0101'0000, Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const volatile), ElementsAre(0b0101'0000, Flags::VOLATILE | Flags::CONST));
  
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *), ElementsAre(0b0101'0000, Flags::PTR));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const), ElementsAre(0b0101'0000, Flags::PTR | Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *volatile), ElementsAre(0b0101'0000, Flags::PTR | Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const volatile), ElementsAre(0b0101'0000, Flags::PTR | Flags::VOLATILE | Flags::CONST));

  ASSERT_THAT(TypeSerializer::serialize_type(^^bool &), ElementsAre(0b0101'0000, Flags::LVREF));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const&), ElementsAre(0b0101'0000, Flags::LVREF | Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool volatile&), ElementsAre(0b0101'0000, Flags::LVREF | Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const volatile&), ElementsAre(0b0101'0000, Flags::LVREF | Flags::VOLATILE | Flags::CONST));

  ASSERT_THAT(TypeSerializer::serialize_type(^^bool &&), ElementsAre(0b0101'0000, Flags::RVREF));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const&&), ElementsAre(0b0101'0000, Flags::RVREF | Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool volatile&&), ElementsAre(0b0101'0000, Flags::RVREF | Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool const volatile&&), ElementsAre(0b0101'0000, Flags::RVREF | Flags::VOLATILE | Flags::CONST));

  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *&), ElementsAre(0b0101'0000, Flags::PTR | Flags::LVREF));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const&), ElementsAre(0b0101'0000, Flags::PTR | Flags::LVREF | Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *volatile&), ElementsAre(0b0101'0000, Flags::PTR | Flags::LVREF | Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const volatile&), ElementsAre(0b0101'0000, Flags::PTR | Flags::LVREF | Flags::VOLATILE | Flags::CONST));

  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *&&), ElementsAre(0b0101'0000, Flags::PTR | Flags::RVREF));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const&&), ElementsAre(0b0101'0000, Flags::PTR | Flags::RVREF | Flags::CONST));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *volatile&&), ElementsAre(0b0101'0000, Flags::PTR | Flags::RVREF | Flags::VOLATILE));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool *const volatile&&), ElementsAre(0b0101'0000, Flags::PTR | Flags::RVREF | Flags::VOLATILE | Flags::CONST));
}

TEST(BinaryTypeSerializer, FlagsMulti) {
  using ::testing::ElementsAre;
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool**),
              ElementsAre(0b0101'0000, Flags::HAS_MORE | Flags::PTR, Flags::PTR));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool* const*),
              ElementsAre(0b0101'0000, Flags::HAS_MORE | Flags::PTR | Flags::CONST, Flags::PTR));
  ASSERT_THAT(TypeSerializer::serialize_type(^^bool* volatile*),
              ElementsAre(0b0101'0000, Flags::HAS_MORE | Flags::PTR | Flags::VOLATILE, Flags::PTR));
}