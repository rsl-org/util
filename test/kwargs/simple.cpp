#include <gtest/gtest.h>
#include <rsl/kwargs>

namespace {
template <typename T>
requires requires(T const& kwargs) {
    { kwargs.x } -> std::convertible_to<int>;
}
int member_access(rsl::kwargs_t<T> const& kwargs) {
  return kwargs.x;
}

template <typename T>
int get_by_name(rsl::kwargs_t<T> const& kwargs) {
  return get<"x">(kwargs);
}

template <typename T>
int get_by_name_default(rsl::kwargs_t<T> const& kwargs) {
  return get_or<"x">(kwargs, 42);
}

template <typename T>
int get_by_idx(rsl::kwargs_t<T> const& kwargs) {
  return get<0>(kwargs);
}

template <typename T>
int get_by_idx_default(rsl::kwargs_t<T> const& kwargs) {
  return get_or<0>(kwargs, 42);
}
}

TEST(KwArgs, Empty) {
  auto args = $args();
  EXPECT_EQ(std::tuple_size_v<decltype(args)>, 0);
}

TEST(KwArgs, Simple) {
  EXPECT_EQ(member_access($args(x=10)), 10);
  EXPECT_EQ(get_by_idx($args(x=10)), 10);
  EXPECT_EQ(get_by_name($args(x=10)), 10);
}

TEST(KwArgs, Default) {
  EXPECT_EQ(get_by_name_default($args(x=10)), 10);
  EXPECT_EQ(get_by_idx_default($args(x=10)), 10);
  EXPECT_EQ(get_by_name_default($args()), 42);
  EXPECT_EQ(get_by_idx_default($args()), 42);
}