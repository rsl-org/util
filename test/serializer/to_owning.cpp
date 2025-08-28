#include <gtest/gtest.h>
#include <common/assertions.h>

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include <rsl/serialize>

#include "../common/assertions.h"

TEST(Serializer, ToOWningVec) {
  ASSERT_SAME(rsl::to_owning<std::vector<int>>, std::vector<int>);

  ASSERT_SAME(rsl::to_owning<std::initializer_list<int>>, std::vector<int>);
  ASSERT_SAME(rsl::to_owning<std::initializer_list<char>>, std::vector<char>);
}

TEST(Serializer, ToOWningStr) {
  ASSERT_SAME(rsl::to_owning<std::string>, std::string);

  ASSERT_SAME(rsl::to_owning<char const*>, std::string);
  ASSERT_SAME(rsl::to_owning<char const[3]>, std::string);
  ASSERT_SAME(rsl::to_owning<std::string_view>, std::string);
}

namespace {
struct TestAggregate {
  char const* name;
  std::string_view test;
  std::vector<std::string_view> zoinks;
};
}  // namespace

TEST(Serializer, ToOwningAggregate) {
  using owning = rsl::to_owning<TestAggregate>;
  ASSERT_SAME(decltype(&owning::name), char const* owning::*);
  ASSERT_SAME(decltype(&owning::test), std::string_view owning::*);
  ASSERT_SAME(decltype(&owning::zoinks), std::vector<std::string_view> owning::*);
}
