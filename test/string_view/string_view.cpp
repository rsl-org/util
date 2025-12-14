#include <gtest/gtest.h>
#include <rsl/string_view>

TEST(StringView, Empty) {
  ASSERT_TRUE(rsl::string_view().empty());
  ASSERT_TRUE(!rsl::string_view("a").empty());
}

TEST(StringView, Size) {
  ASSERT_EQ(rsl::string_view().size(), 0);
  ASSERT_EQ(rsl::string_view("a").size(), 1);
  ASSERT_EQ(rsl::string_view("abc").size(), 3);
}

TEST(StringView, Compare) {
  ASSERT_EQ(rsl::string_view(), rsl::string_view());
  ASSERT_EQ(rsl::string_view("a"), rsl::string_view("a"));
  ASSERT_NE(rsl::string_view("a"), rsl::string_view());
  ASSERT_NE(rsl::string_view("a"), rsl::string_view("b"));
}

TEST(StringView, Misc) {
  ASSERT_EQ(sizeof(rsl::string_view::value_type), 1);
  ASSERT_EQ(rsl::string_view("a").max_size(), std::numeric_limits<size_t>::max());
}
