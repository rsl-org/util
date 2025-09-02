#include <gtest/gtest.h>
#include <rsl/span>

#include <span>

TEST(span, construct) {
  constexpr int ca[]{0, 1, 2, 3, 4, 5, 6, 7, 8};
  rsl::span span(ca);
  rsl::span<const int, 3> spanSized(ca, 3);

  constexpr std::array arr{0, 1, 2, 3, 4, 5, 6, 7, 8};
  rsl::span spanFromArray(arr);
  rsl::span<const int, 3> spanSizedFromArray(arr.begin(), 3);

  constexpr auto objects =
      define_static_array(members_of(^^::, std::meta::access_context::current()));

  constexpr rsl::span spanOfInfo(objects);
  constexpr rsl::span<const std::meta::info, 3> spanOfInfoSized(objects.data(), 3);

  /* does not compile yet
  {
    auto subspan         = span.subspan<2>();
    auto subspanWithSize = span.subspan<1, 2>();
  }
  {
    auto subspan         = spanSized.subspan<2>();
    auto subspanWithSize = spanSized.subspan<1, 2>();
  }
  {
    auto subspan         = spanFromArray.subspan<2>();
    auto subspanWithSize = spanFromArray.subspan<1, 2>();
  }
  {
    auto subspan         = spanSizedFromArray.subspan<2>();
    auto subspanWithSize = spanSizedFromArray.subspan<1, 2>();
  }
  consteval {
      auto subspan         = spanOfInfo.subspan<2>();
      auto subspanWithSize = spanOfInfo.subspan<1, 2>();
  };
   */
}

TEST(span, Empty) {
  constexpr rsl::span<int> empty;
  ASSERT_TRUE(empty.empty());

  constexpr int a[]{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto span = rsl::span{a};
  ASSERT_TRUE(!span.empty());

  constexpr auto spanOfInfo =
      rsl::span{define_static_array(members_of(^^::, std::meta::access_context::current()))};
  static_assert(!spanOfInfo.empty());
}

TEST(span, Size) {
  constexpr rsl::span<int> empty;
  ASSERT_EQ(empty.size(), 0);

  constexpr int a[]{0, 1, 2, 3, 4, 5, 6, 7, 8};
  auto span = rsl::span{a};
  ASSERT_EQ(span.size(), 9);

  auto spanWithSize = rsl::span{a, 3};
  ASSERT_EQ(spanWithSize.size(), 3);

  constexpr auto spanOfInfo =
      rsl::span{define_static_array(members_of(^^::, std::meta::access_context::current()))};
  static_assert(spanOfInfo.size() > 1);
}
