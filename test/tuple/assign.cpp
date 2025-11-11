#include <rsl/tuple>
#include <common/assertions.h>
#include <gtest/gtest.h>

struct B {
  int id_;
  explicit B(int i = 0) : id_(i) {}
  B(const B&)            = default;
  B& operator=(const B&) = default;
  virtual ~B() {}
};

struct D : B {
  explicit D(int i) : B(i) {}
};

struct MoveOnly {
  MoveOnly() : val(0) {}
  MoveOnly(int v) : val(v) {}
  MoveOnly(MoveOnly&& other) noexcept : val(other.val) { other.val = -1; }
  MoveOnly& operator=(MoveOnly&& other) noexcept {
    val       = other.val;
    other.val = -1;
    return *this;
  }
  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  int val;
};

template <class T>
struct AssignableFrom {
  T v;

  constexpr AssignableFrom() = default;

  template <class U>
  constexpr AssignableFrom(U&& u)
    requires std::is_constructible_v<T, U&&>
      : v(std::forward<U>(u)) {}

  constexpr AssignableFrom& operator=(const T& t)
    requires std::is_copy_assignable_v<T>
  {
    v = t;
    return *this;
  }

  constexpr AssignableFrom& operator=(T&& t)
    requires std::is_move_assignable_v<T>
  {
    v = std::move(t);
    return *this;
  }

  constexpr const AssignableFrom& operator=(const T& t) const
    requires std::is_assignable_v<const T&, const T&>
  {
    v = t;
    return *this;
  }

  constexpr const AssignableFrom& operator=(T&& t) const
    requires std::is_assignable_v<const T&, T&&>
  {
    v = std::move(t);
    return *this;
  }
};

struct ConstCopyAssign {
  mutable int val;

  constexpr ConstCopyAssign() = default;
  constexpr ConstCopyAssign(int v) : val(v) {}

  constexpr const ConstCopyAssign& operator=(const ConstCopyAssign& other) const {
    val = other.val;
    return *this;
  }

  constexpr ConstCopyAssign& operator=(const ConstCopyAssign&)        = delete;
  constexpr ConstCopyAssign& operator=(ConstCopyAssign&&)             = delete;
  constexpr const ConstCopyAssign& operator=(ConstCopyAssign&&) const = delete;
};

struct ConstMoveAssign {
  mutable int val;

  constexpr ConstMoveAssign() = default;
  constexpr ConstMoveAssign(int v) : val(v) {}

  constexpr const ConstMoveAssign& operator=(ConstMoveAssign&& other) const {
    val = other.val;
    return *this;
  }

  constexpr ConstMoveAssign& operator=(const ConstMoveAssign&)             = delete;
  constexpr const ConstMoveAssign& operator=(const ConstMoveAssign&) const = delete;
  constexpr ConstMoveAssign& operator=(ConstMoveAssign&&)                  = delete;
};

struct TracedAssignment {
  int copyAssign              = 0;
  mutable int constCopyAssign = 0;
  int moveAssign              = 0;
  mutable int constMoveAssign = 0;

  constexpr TracedAssignment() = default;

  constexpr TracedAssignment& operator=(const TracedAssignment&) {
    copyAssign++;
    return *this;
  }
  constexpr const TracedAssignment& operator=(const TracedAssignment&) const {
    constCopyAssign++;
    return *this;
  }
  constexpr TracedAssignment& operator=(TracedAssignment&&) {
    moveAssign++;
    return *this;
  }
  constexpr const TracedAssignment& operator=(TracedAssignment&&) const {
    constMoveAssign++;
    return *this;
  }
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
    t = t2;
    ASSERT_TRUE(rsl::get<0>(t) == x2);
    ASSERT_TRUE(&rsl::get<0>(t) == &x);
    ASSERT_TRUE(rsl::get<1>(t) == y2);
    ASSERT_TRUE(&rsl::get<1>(t) == &y);
  }
  {
    int i1    = 1;
    int i2    = 2;
    double d1 = 3.0;
    double d2 = 5.0;
    rsl::tuple<int&, double&> t1{i1, d1};
    const rsl::tuple<int&, double&> t2{i2, d2};
    t2 = std::move(t1);
    ASSERT_EQ(i1, 1);
    ASSERT_EQ(i2, 1);
    ASSERT_EQ(d1, 3.0);
    ASSERT_EQ(d2, 3.0);
    ASSERT_EQ(rsl::get<0>(t2), 1);
    ASSERT_EQ(rsl::get<1>(t2), 3.0);
  }
  {
    using T = rsl::tuple<MoveOnly>;
    T t0(MoveOnly(0));
    T t;
    t = std::move(t0);
    ASSERT_EQ(rsl::get<0>(t).val, 0);
  }
  {
    const rsl::tuple<ConstCopyAssign> t1{1};
    const rsl::tuple<ConstCopyAssign> t2{2};
    t2 = t1;
    ASSERT_EQ(rsl::get<0>(t2).val, 1);
  }
  {
    rsl::tuple<ConstMoveAssign> t1{1};
    const rsl::tuple<ConstMoveAssign> t2{2};
    t2 = std::move(t1);
    ASSERT_EQ(rsl::get<0>(t2).val, 1);
  }
  {
    using T = rsl::tuple<MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1));
    T t;
    t = std::move(t0);
    ASSERT_EQ(rsl::get<0>(t).val, 0);
    ASSERT_EQ(rsl::get<1>(t).val, 1);
  }
  {
    using T = rsl::tuple<MoveOnly, MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1), MoveOnly(2));
    T t;
    t = std::move(t0);
    ASSERT_EQ(rsl::get<0>(t).val, 0);
    ASSERT_EQ(rsl::get<1>(t).val, 1);
    ASSERT_EQ(rsl::get<2>(t).val, 2);
  }
  {
    rsl::tuple<TracedAssignment, const TracedAssignment> t1{};
    const rsl::tuple<TracedAssignment, const TracedAssignment> t2{};
    t2 = t1;
    ASSERT_EQ(rsl::get<0>(t2).constCopyAssign, 1);
    ASSERT_EQ(rsl::get<1>(t2).constCopyAssign, 1);
  }
  {
    rsl::tuple<TracedAssignment, const TracedAssignment> t1{};
    const rsl::tuple<TracedAssignment, const TracedAssignment> t2{};
    t2 = std::move(t1);
    ASSERT_EQ(rsl::get<0>(t2).constMoveAssign, 1);
    ASSERT_EQ(rsl::get<1>(t2).constCopyAssign, 1);
  }
}

TEST(Tuple, ConvertingAssign) {
  {
    using T0 = rsl::tuple<long>;
    using T1 = rsl::tuple<long long>;
    T0 t0(2);
    T1 t1;
    t1 = t0;
    ASSERT_EQ(rsl::get<0>(t1), 2);
  }

  {
    using T0 = rsl::tuple<long, char>;
    using T1 = rsl::tuple<long long, int>;
    T0 t0(2, 'a');
    T1 t1;
    t1 = t0;
    ASSERT_EQ(rsl::get<0>(t1), 2);
    ASSERT_EQ(rsl::get<1>(t1), int('a'));
  }
  {
    using T0 = rsl::tuple<long, char, D>;
    using T1 = rsl::tuple<long long, int, B>;
    T0 t0(2, 'a', D(3));
    T1 t1;
    t1 = t0;
    assert(rsl::get<0>(t1) == 2);
    assert(rsl::get<1>(t1) == int('a'));
    assert(rsl::get<2>(t1).id_ == 3);
  }
  {
    D d(3);
    D d2(2);
    using T0 = rsl::tuple<long, char, D&>;
    using T1 = rsl::tuple<long long, int, B&>;
    T0 t0(2, 'a', d2);
    T1 t1(1, 'b', d);
    t1 = t0;
    ASSERT_EQ(rsl::get<0>(t1), 2);
    ASSERT_EQ(rsl::get<1>(t1), int('a'));
    ASSERT_EQ(rsl::get<2>(t1).id_, 2);
  }
  {
    // Test that tuple evaluates correctly applies an lvalue reference
    // before evaluating is_assignable (i.e. 'is_assignable<int&, int&>')
    // instead of evaluating 'is_assignable<int&&, int&>' which is false.
    int x = 42;
    int y = 43;
    rsl::tuple<int&&> t(std::move(x));
    rsl::tuple<int&> t2(y);
    t = t2;
    ASSERT_TRUE(rsl::get<0>(t) == 43);
    ASSERT_TRUE(&rsl::get<0>(t) == &x);
  }
  {
    using T0 = rsl::tuple<long>;
    using T1 = rsl::tuple<long long>;
    T0 t0(2);
    T1 t1;
    t1 = std::move(t0);
    assert(std::get<0>(t1) == 2);
  }
  {
    using T0 = rsl::tuple<long, char>;
    using T1 = rsl::tuple<long long, int>;
    T0 t0(2, 'a');
    T1 t1;
    t1 = std::move(t0);
    ASSERT_EQ(rsl::get<0>(t1), 2);
    ASSERT_EQ(rsl::get<1>(t1), int('a'));
  }

  {
    int i1  = 1;
    int i2  = 2;
    long j1 = 3;
    long j2 = 4;
    const rsl::tuple<int&, int&> t1{i1, i2};
    const rsl::tuple<long&, long&> t2{j1, j2};
    t2 = t1;
    ASSERT_EQ(rsl::get<0>(t2), 1);
    ASSERT_EQ(rsl::get<1>(t2), 2);
  }

  {
    const rsl::tuple<ConstCopyAssign> t1{1};
    const rsl::tuple<AssignableFrom<ConstCopyAssign>> t2{2};
    t2 = t1;
    ASSERT_EQ(rsl::get<0>(t2).v.val, 1);
  }

  {
    rsl::tuple<TracedAssignment> t1{};
    const rsl::tuple<AssignableFrom<TracedAssignment>> t2{};
    t2 = t1;
    ASSERT_EQ(rsl::get<0>(t2).v.constCopyAssign, 1);
  }

  {
    int i1  = 1;
    int i2  = 2;
    long j1 = 3;
    long j2 = 4;
    rsl::tuple<int&, int&> t1{i1, i2};
    const rsl::tuple<long&, long&> t2{j1, j2};
    t2 = std::move(t1);
    ASSERT_EQ(rsl::get<0>(t2), 1);
    ASSERT_EQ(rsl::get<1>(t2), 2);
  }

  {
    rsl::tuple<ConstMoveAssign> t1{1};
    const rsl::tuple<AssignableFrom<ConstMoveAssign>> t2{2};
    t2 = std::move(t1);
    ASSERT_EQ(rsl::get<0>(t2).v.val, 1);
  }

  {
    rsl::tuple<TracedAssignment> t1{};
    const rsl::tuple<AssignableFrom<TracedAssignment>> t2{};
    t2 = std::move(t1);
    ASSERT_EQ(rsl::get<0>(t2).v.constMoveAssign, 1);
  }
}
