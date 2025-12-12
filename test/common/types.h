#pragma once

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
