#include <string_view>
#include <vector>
#include <source_location>

#include <gtest/gtest.h>
#include <rsl/kwargs>

namespace {
void check(std::string_view str,
           std::vector<std::string_view> const& expected_names,
           std::source_location const& loc = std::source_location::current()) {
  auto parser          = rsl::kwargs::NameParser{str};
  auto source_location = std::format("{}:{}", loc.file_name(), loc.line());
  EXPECT_TRUE(parser.parse()) << source_location;

  EXPECT_EQ(parser.names.size(), expected_names.size()) << source_location;
  for (auto [parsed, expected] : std::views::zip(parser.names, expected_names)) {
    EXPECT_EQ(parsed, expected) << source_location;
  }
}

void check_reject(std::string_view str,
                  std::source_location const& loc = std::source_location::current()) {
  auto parser          = rsl::kwargs::NameParser{str};
  auto source_location = std::format("{}:{}", loc.file_name(), loc.line());
  EXPECT_FALSE(parser.parse()) << source_location;
}
}  // namespace

TEST(CaptureList, Named) {
  std::vector<std::string_view> one_arg = {"x=1", "x = 1", "x= 1", "x =1"};

  for (auto capture_list : one_arg) {
    check(capture_list, {"x"});
  }

  std::vector<std::string_view> two_args = {"x = 1, y = 1", "x=1,y=1", "x=1 ,y=1", "x=1 , y=1"};

  for (auto capture_list : two_args) {
    check(capture_list, {"x", "y"});
  }
}

TEST(CaptureList, Braces) {
  std::vector<std::string_view> two_args = {
      "x = f(a,b(c, d)), y=2",
      "x = f{a, b{c, d}}, y=1",
      "x = f[a, b[c, d]], y=1",
      "x = f{a(b[c(d{1, 3}, 5), 2], 3), b{c{2, 3}, d}}, y=1",
  };

  for (auto capture_list : two_args) {
    check(capture_list, {"x", "y"});
  }
}

TEST(CaptureList, Unnamed) {
  std::vector<std::string_view> three_args = {"x, y =2, z=3", "x = 2, y, z=2", "x =2, y=3, z"};

  for (auto capture_list : three_args) {
    check(capture_list, {"x", "y", "z"});
  }
}
