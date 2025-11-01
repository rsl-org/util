#include <rsl/serializer/binary/type/deserializer.hpp>
#include <rsl/serializer/binary/type/serializer.hpp>

#include <gtest/gtest.h>

using namespace rsl::serializer::_binary_impl;

TEST(BinarySerializer, String) {
  auto result = serialize_string("foo");
  auto expected_data = std::vector<std::uint8_t>{{3, 'f', 'o', 'o'}};
  
  ASSERT_EQ(result, expected_data);
  ASSERT_EQ("foo", get<1>(deserialize_string(result)));
}
