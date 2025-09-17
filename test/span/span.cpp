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

template <typename ReferenceT>
constexpr void testSpanAt(auto&& anySpan, int index, auto expectedValue) {
  // non-const
  {
    std::same_as<ReferenceT> decltype(auto) elem = anySpan.at(index);
    ASSERT_EQ(elem, expectedValue);
  }

  // const
  {
    std::same_as<ReferenceT> decltype(auto) elem = std::as_const(anySpan).at(index);
    ASSERT_EQ(elem, expectedValue);
  }
}

TEST(span, at) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};

    ASSERT_TRUE(std::dynamic_extent != arrSpan.extent);

    using ReferenceT = typename decltype(arrSpan)::reference;

    testSpanAt<ReferenceT>(arrSpan, 0, 0);
    testSpanAt<ReferenceT>(arrSpan, 1, 1);
    testSpanAt<ReferenceT>(arrSpan, 6, 9084);
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};

    ASSERT_TRUE(std::dynamic_extent == vecSpan.extent);

    using ReferenceT = typename decltype(vecSpan)::reference;

    testSpanAt<ReferenceT>(vecSpan, 0, 0);
    testSpanAt<ReferenceT>(vecSpan, 1, 1);
    testSpanAt<ReferenceT>(vecSpan, 6, 9084);
  }

  {
    constexpr auto spanOfInfo =
        rsl::span{define_static_array(members_of(^^::, std::meta::access_context::current()))};
    static_assert(spanOfInfo.at(1) == members_of(^^::, std::meta::access_context::current())[1]);
    static_assert(spanOfInfo.at(2) != members_of(^^::, std::meta::access_context::current())[1]);
    static_assert(spanOfInfo.at(2) == members_of(^^::, std::meta::access_context::current())[2]);
  }
}

TEST(span, back) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};
    ASSERT_EQ(arrSpan.back(), 9084);
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};
    ASSERT_EQ(vecSpan.back(), 9084);
  }

  {
    constexpr auto members =
        define_static_array(members_of(^^::, std::meta::access_context::current()));
    constexpr auto spanOfInfo = rsl::span{members};
    static_assert(spanOfInfo.back() == members[members.size() - 1]);
  }
}

TEST(span, front) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};
    ASSERT_EQ(arrSpan.front(), 0);
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};
    ASSERT_EQ(vecSpan.front(), 0);
  }

  {
    constexpr auto members =
        define_static_array(members_of(^^::, std::meta::access_context::current()));
    constexpr auto spanOfInfo = rsl::span{members};
    static_assert(spanOfInfo.front() == members[0]);
  }
}

TEST(span, size_bytes) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};
    ASSERT_EQ(arrSpan.size_bytes(), arr.size() * sizeof(int));
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};
    ASSERT_EQ(vecSpan.size_bytes(), vecSpan.size() * sizeof(int));
  }

  {
    constexpr auto members =
        define_static_array(members_of(^^::, std::meta::access_context::current()));
    constexpr auto spanOfInfo = rsl::span{members};
    static_assert(spanOfInfo.size_bytes() == members.size() * sizeof(std::meta::info));
  }
}

TEST(span, first) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};
    const auto firstThree = arrSpan.first(3);
    ASSERT_EQ(firstThree.size(), 3);
    ASSERT_EQ(firstThree[1], 1);
    ASSERT_EQ(arrSpan.first<2>().size(), 2);
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};
    auto const firstThree = vecSpan.first(3);
    ASSERT_EQ(firstThree.size(), 3);
    ASSERT_EQ(firstThree[1], 1);
    ASSERT_EQ(vecSpan.first<2>().size(), 2);
  }

  {
    constexpr auto members =
        define_static_array(members_of(^^::, std::meta::access_context::current()));
    constexpr auto spanOfInfo = rsl::span{members};
    static_assert(spanOfInfo.first<3>().size() == 3);
  }
}

TEST(span, last) {
  // With static extent
  {
    std::array arr{0, 1, 2, 3, 4, 5, 9084};
    rsl::span arrSpan{arr};
    auto lastThree = arrSpan.last(3);
    ASSERT_EQ(lastThree.size(), 3);
    ASSERT_EQ(lastThree[2], 9084);
    ASSERT_EQ(arrSpan.last<2>().size(), 2);
  }

  // With dynamic extent
  {
    std::vector vec{0, 1, 2, 3, 4, 5, 9084};
    rsl::span vecSpan{vec};
    auto const lastThree = vecSpan.last(3);
    ASSERT_EQ(lastThree.size(), 3);
    ASSERT_EQ(lastThree[2], 9084);
    ASSERT_EQ(vecSpan.last<2>().size(), 2);
  }

  {
    constexpr auto members =
        define_static_array(members_of(^^::, std::meta::access_context::current()));
    constexpr auto spanOfInfo = rsl::span{members};
    static_assert(spanOfInfo.last<3>().size() == 3);
  }
}
