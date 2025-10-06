#include <gtest/gtest.h>
#include <rsl/variant>

#include <common/assertions.h>

TEST(Get, IndexLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant obj(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(obj));
    ASSERT_SAME(decltype(rsl::get<0>(obj)), const int&);
    static_assert(rsl::get<0>(obj) == 42, "Value mismatch");
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(rsl::get<1>(v)), long&);
    ASSERT_EQ(rsl::get<1>(v), 42l);
  }
}
TEST(Get, IndexRvalue) {
  using variant = rsl::variant<int, long>;
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(std::move(v)));
    ASSERT_SAME(decltype(rsl::get<0>(std::move(v))), int&&);
    ASSERT_EQ(rsl::get<0>(std::move(v)), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(rsl::get<1>(std::move(v))), long&&);
    ASSERT_EQ(rsl::get<1>(std::move(v)), 42l);
  }
}

TEST(Get, IndexConstLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(v));
    ASSERT_SAME(decltype(rsl::get<0>(v)), const int&);
    static_assert(rsl::get<0>(v) == 42, "");
  }
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(v));
    ASSERT_SAME(decltype(rsl::get<0>(v)), const int&);
    ASSERT_EQ(rsl::get<0>(v), 42);
  }
  {
    constexpr variant v(42l);
    ASSERT_NOT_NOEXCEPT(rsl::get<1>(v));
    ASSERT_SAME(decltype(rsl::get<1>(v)), const long&);
    static_assert(rsl::get<1>(v) == 42, "");
  }
  {
    const variant v(42l);
    ASSERT_NOT_NOEXCEPT(rsl::get<1>(v));
    ASSERT_SAME(decltype(rsl::get<1>(v)), const long&);
    ASSERT_EQ(rsl::get<1>(v), 42);
  }
}

TEST(Get, IndexConstRvalue) {
  using variant = rsl::variant<int, long>;
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(std::move(v)));
    ASSERT_SAME(decltype(rsl::get<0>(std::move(v))), const int&&);
    ASSERT_EQ(rsl::get<0>(std::move(v)), 42);
  }
  {
    const variant v(42l);
    ASSERT_SAME(decltype(rsl::get<1>(std::move(v))), const long&&);
    ASSERT_EQ(rsl::get<1>(std::move(v)), 42);
  }
}

TEST(Get, IndexException) {
  // TODO
}

TEST(Get, TypeLvalue) {
  using variant = rsl::variant<int, long>;
  static_assert(std::same_as<rsl::variant_alternative<0, variant>::type, int>);
  static_assert(std::same_as<rsl::variant_alternative<1, variant>::type, long>);
  static_assert(
      rsl::_variant_impl::variant_base<rsl::_impl::Storage<int, long>>::alternatives.get_index_of(
          ^^int) == 0);
  static_assert(
      rsl::_variant_impl::variant_base<rsl::_impl::Storage<int, long>>::alternatives.get_index_of(
          ^^long) == 1);
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<int>(v));
    ASSERT_SAME(decltype(rsl::get<int>(v)), int&);
    ASSERT_SAME(decltype(rsl::get<int&>(v)), int&);
    ASSERT_SAME(decltype(rsl::get<const int&>(v)), int&);
    ASSERT_SAME(decltype(rsl::get<int&&>(v)), int&);
    ASSERT_EQ(rsl::get<int>(v), 42);
    ASSERT_EQ(rsl::get<int&>(v), 42);
    ASSERT_EQ(rsl::get<const int&>(v), 42);
    ASSERT_EQ(rsl::get<int&&>(v), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(rsl::get<long>(v)), long&);
    ASSERT_SAME(decltype(rsl::get<const long&>(v)), long&);
    ASSERT_EQ(rsl::get<long>(v), 42);
    ASSERT_EQ(rsl::get<const long&>(v), 42);
  }
}

TEST(Get, TypeRvalue) {
  using variant = rsl::variant<int, long>;
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<int>(std::move(v)));
    ASSERT_SAME(decltype(rsl::get<int>(std::move(v))), int&&);
    ASSERT_SAME(decltype(rsl::get<const int&>(std::move(v))), int&&);
    ASSERT_SAME(decltype(rsl::get<int&&>(std::move(v))), int&&);
    ASSERT_EQ(rsl::get<int>(std::move(v)), 42);
    ASSERT_EQ(rsl::get<const int&>(std::move(v)), 42);
    ASSERT_EQ(rsl::get<int&&>(std::move(v)), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(rsl::get<long>(std::move(v))), long&&);
    ASSERT_EQ(rsl::get<long>(std::move(v)), 42);
  }
}

TEST(Get, TypeConstRvalue) {
  using variant = rsl::variant<int, long>;
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<int>(std::move(v)));
    ASSERT_SAME(decltype(rsl::get<int>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(rsl::get<const int&>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(rsl::get<const int&&>(std::move(v))), const int&&);
    ASSERT_EQ(rsl::get<int>(std::move(v)), 42);
    ASSERT_EQ(rsl::get<const int&>(std::move(v)), 42);
    ASSERT_EQ(rsl::get<const int&&>(std::move(v)), 42);
  }
  {
    const variant v(42l);
    ASSERT_SAME(decltype(rsl::get<long>(std::move(v))), const long&&);
    ASSERT_EQ(rsl::get<long>(std::move(v)), 42);
  }
}

TEST(Get, TypeConstLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<int>(v));
    ASSERT_SAME(decltype(rsl::get<int>(v)), const int&);
    ASSERT_SAME(decltype(rsl::get<int&>(v)), const int&);
    ASSERT_SAME(decltype(rsl::get<const int&>(v)), const int&);
    ASSERT_SAME(decltype(rsl::get<int&&>(v)), const int&);
    static_assert(rsl::get<int>(v) == 42, "");
  }
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<int>(v));
    ASSERT_SAME(decltype(rsl::get<int>(v)), const int&);
    ASSERT_EQ(rsl::get<int>(v), 42);
  }
  {
    constexpr variant v(42l);
    ASSERT_NOT_NOEXCEPT(rsl::get<long>(v));
    ASSERT_SAME(decltype(rsl::get<long>(v)), const long&);
    static_assert(rsl::get<long>(v) == 42, "");
  }
  {
    const variant v(42l);
    ASSERT_NOT_NOEXCEPT(rsl::get<long>(v));
    ASSERT_SAME(decltype(rsl::get<long>(v)), const long&);
    ASSERT_EQ(rsl::get<long>(v), 42);
  }
}
