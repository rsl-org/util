#include <vector>
#include <map>
#include <string>
#include <string_view>

#include <gtest/gtest.h>
#include <common/assertions.h>

#include <rsl/serialize>
#include "common/util.h"

TEST(ToBytes, Integral) {
  ASSERT_EQ(rsl::to_bytes(true), std::vector<std::byte>(1, std::byte(1)));
  ASSERT_EQ(rsl::to_bytes(false), std::vector<std::byte>(1));
  ASSERT_EQ(rsl::to_bytes(0), std::vector<std::byte>(sizeof(int)));
  ASSERT_EQ(rsl::to_bytes(0ZU), std::vector<std::byte>(sizeof(std::size_t)));
  ASSERT_EQ(rsl::to_bytes(0L), std::vector<std::byte>(sizeof(long)));
  ASSERT_EQ(rsl::to_bytes(0LL), std::vector<std::byte>(sizeof(long long)));
  ASSERT_EQ(rsl::to_bytes(0U), std::vector<std::byte>(sizeof(unsigned)));
  ASSERT_EQ(rsl::to_bytes(0UL), std::vector<std::byte>(sizeof(unsigned long)));
  ASSERT_EQ(rsl::to_bytes(0ULL), std::vector<std::byte>(sizeof(unsigned long long)));
  // ASSERT_EQ(rsl::repr(1.2), "1.2");
  // ASSERT_EQ(rsl::repr(1.2F), "1.2F");
  // ASSERT_EQ(rsl::repr(1.2L), "1.2L");
}

TEST(ToBytes, Strings) {
  ASSERT_EQ(rsl::to_bytes('0'), test_util::to_bytes("0"));

  auto result = test_util::bytes(1, 0, 0, 0, 0, 0, 0, 0, '0');

  ASSERT_EQ(rsl::to_bytes("0"), result);
  ASSERT_EQ(rsl::to_bytes(std::string("0")), result);
  ASSERT_EQ(rsl::to_bytes(std::string_view("0")), result);
}

// namespace {
// namespace TestNS {
// struct TestAgg {
//   int a;
//   char b;
// };

// struct Unsupported{
//   explicit Unsupported(int){}
// };
// }  // namespace TestNS
// }  // namespace

// TEST(Repr, StringLikes) {
//   char const* var = "foo";
//   ASSERT_EQ(rsl::repr("foo"), "\"foo\"");
//   ASSERT_EQ(rsl::repr(var), "\"foo\"");
//   ASSERT_EQ(rsl::repr(std::string("foo")), "\"foo\"");
//   ASSERT_EQ(rsl::repr(std::string_view("foo")), "\"foo\"");

//   static_assert(rsl::repr(std::string_view("foo")) == "\"foo\"");
// }

// TEST(Repr, Aggregates) {
//   auto obj = TestNS::TestAgg{42, 'c'};
//   ASSERT_EQ(rsl::repr(obj), "TestAgg{42, 'c'}");
//   ASSERT_EQ(rsl::repr(obj, {.names=rsl::NameMode::qualified}), "TestNS::TestAgg{42, 'c'}");
// }

// TEST(Repr, Iterables) {
//   ASSERT_EQ(rsl::repr(std::vector<int>({1, 2, 3, 4})), "vector<int>{1, 2, 3, 4}");
//   ASSERT_EQ(rsl::repr(std::vector<int>({1, 2, 3, 4}), {.names=rsl::NameMode::qualified}),
//   "std::vector<int>{1, 2, 3, 4}");

//   ASSERT_EQ(rsl::repr(std::map<int, int>({{1, 2}, {3, 4}})), "map<int, int>{{1, 2}, {3, 4}}");
// }

// TEST(Repr, Unsupported) {
//   ASSERT_EQ(rsl::repr(TestNS::Unsupported(3)), "Unsupported{/*...*/}");
// }