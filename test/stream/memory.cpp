#include <gtest/gtest.h>

#include <vector>
#include <array>
#include <cstddef>
#include <cstring>

#include <common/assertions.h>
#include <common/util.h>

#include <rsl/stream/memory.hpp>

namespace rsl::stream {
TEST(MemoryInputStreamTest, ReadsFullyWhenEnoughSpace) {
  auto data = test_util::to_bytes("hello");
  MemoryInputStream mis(data);

  std::array<std::byte, 5> buf{};
  auto [n, status] = mis.read(buf.data(), buf.size());

  EXPECT_EQ(status, IOStatus::ok);
  EXPECT_EQ(n, 5U);
  EXPECT_MEM_EQ(buf.data(), "hello", 5);
}

TEST(MemoryInputStreamTest, ReadsPartiallyWhenBufferSmaller) {
  auto data = test_util::to_bytes("abcdef");
  MemoryInputStream mis(data);

  std::array<std::byte, 3> buf{};
  auto [n, status] = mis.read(buf.data(), buf.size());

  EXPECT_EQ(status, IOStatus::ok);
  EXPECT_EQ(n, 3U);
  EXPECT_MEM_EQ(buf.data(), "abc", 3);
}

TEST(MemoryInputStreamTest, SubsequentReadsAdvanceCursor) {
  auto data = test_util::to_bytes("abcdef");
  MemoryInputStream mis(data);

  std::array<std::byte, 3> buf1{};
  std::array<std::byte, 3> buf2{};

  auto [n1, status1] = mis.read(buf1.data(), buf1.size());
  auto [n2, status2] = mis.read(buf2.data(), buf2.size());

  EXPECT_EQ(status1, IOStatus::ok);
  EXPECT_EQ(status2, IOStatus::ok);

  EXPECT_EQ(n1, 3U);
  EXPECT_EQ(n2, 3U);

  EXPECT_MEM_EQ(buf1.data(), "abc", 3);
  EXPECT_MEM_EQ(buf2.data(), "def", 3);
}

TEST(MemoryInputStreamTest, ReadingPastEndReturnsZero) {
  auto data = test_util::to_bytes("abc");
  MemoryInputStream mis(data);

  std::array<std::byte, 5> buf{};
  auto [n1, status1] = mis.read(buf.data(), buf.size());
  auto [n2, status2] = mis.read(buf.data(), buf.size());

  EXPECT_EQ(status1, IOStatus::ok);
  EXPECT_EQ(status2, IOStatus::end_of_stream);

  EXPECT_EQ(n1, 3U);
  EXPECT_EQ(n2, 0U);
}

TEST(VectorOutputStreamTest, WritesToUnderlyingVector) {
  std::vector<std::byte> storage;
  VectorOutputStream vos(storage);

  auto src         = test_util::to_bytes("xyz");
  auto [n, status] = vos.write(src.data(), src.size());

  EXPECT_EQ(status, IOStatus::ok);
  EXPECT_EQ(n, 3U);
  ASSERT_EQ(storage.size(), 3U);
  EXPECT_MEM_EQ(storage.data(), "xyz", 3);
}

TEST(VectorOutputStreamTest, AppendsOnMultipleWrites) {
  std::vector<std::byte> storage;
  VectorOutputStream vos(storage);

  auto src1 = test_util::to_bytes("hi");
  auto src2 = test_util::to_bytes("there");

  vos.write(src1.data(), src1.size());
  vos.write(src2.data(), src2.size());

  ASSERT_EQ(storage.size(), 7U);
  EXPECT_MEM_EQ(storage.data(), "hithere", 7);
}

TEST(VectorOutputStreamTest, ReserveExpandsCapacity) {
  std::vector<std::byte> storage;
  VectorOutputStream vos(storage);

  auto old_capacity = storage.capacity();
  vos.reserve(100);
  EXPECT_GE(storage.capacity(), 100U);
  EXPECT_GE(storage.capacity(), old_capacity);
}
}  // namespace rsl::stream