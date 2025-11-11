#include <rsl/tuple>
#include <common/assertions.h>
#include <gtest/gtest.h>

struct MoveOnly {
  MoveOnly() : val(0) {}
  MoveOnly(int v) : val(v) {}
  MoveOnly(MoveOnly&& other) noexcept : val(other.val) { other.val = -1; }
  MoveOnly& operator=(MoveOnly&& other) noexcept {
    val        = other.val;
    other.val  = -1;
    return *this;
  }
  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  int val;
};


TEST(Tuple, Assign_traits) {
  static_assert(!std::is_assignable_v<const rsl::tuple<int>&, const rsl::tuple<int>&>);
  static_assert(std::is_assignable_v<const rsl::tuple<int&>&, const rsl::tuple<int&>&>);
  static_assert(std::is_assignable_v<const rsl::tuple<int&, int&>&, const rsl::tuple<int&, int&>&>);
  static_assert(!std::is_assignable_v<const rsl::tuple<int&, int>&, const rsl::tuple<int&, int>&>);
}

TEST(Tuple, Assign) {
  {
    using T = rsl::tuple<>;
    T t0;
    T t;
    t = t0;
  }
  {
    using T = rsl::tuple<int&, int&&>;
    int x   = 42;
    int y   = 100;
    int x2  = -1;
    int y2  = 500;
    T t(x, std::move(y));
    T t2(x2, std::move(y2));
    // TODO: make this work
    // t = t2;
    // ASSERT_TRUE(rsl::get<0>(t) == x2);
    // ASSERT_TRUE(&rsl::get<0>(t) == &x);
    // ASSERT_TRUE(rsl::get<1>(t) == y2);
    // ASSERT_TRUE(&rsl::get<1>(t) == &y);
  }

  {
    using T =  rsl::tuple<MoveOnly>;
    T t0(MoveOnly(0));
    T t;
    t = std::move(t0);
    assert(std::get<0>(t) == 0);
  }
  {
    using T =  rsl::tuple<MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1));
    T t;
    t = std::move(t0);
    assert(std::get<0>(t) == 0);
    assert(std::get<1>(t) == 1);
  }
  {
    using T = rsl::tuple<MoveOnly, MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1), MoveOnly(2));
    T t;
    t = std::move(t0);
    assert(std::get<0>(t) == 0);
    assert(std::get<1>(t) == 1);
    assert(std::get<2>(t) == 2);
  }
}
