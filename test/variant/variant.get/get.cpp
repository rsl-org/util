#include <gtest/gtest.h>
#include <rsl/variant>

#include <common/assertions.h>

TEST(Get, IndexLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant obj(42);
    ASSERT_NOT_NOEXCEPT(get<0>(obj));
    ASSERT_SAME(decltype(get<0>(obj)), const int&);
    static_assert(get<0>(obj) == 42, "Value mismatch");
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(get<1>(v)), long&);
    ASSERT_EQ(get<1>(v), 42l);
  }

  using variant_const = rsl::variant<const int, long>;
  {
    constexpr variant_const obj(42);
    ASSERT_NOT_NOEXCEPT(get<0>(obj));
    ASSERT_SAME(decltype(get<0>(obj)), const int&);
    static_assert(get<0>(obj) == 42, "Value mismatch");
  }
  {
    variant_const v(42l);
    ASSERT_SAME(decltype(get<1>(v)), long&);
    ASSERT_EQ(get<1>(v), 42l);
  }
}

TEST(Get, IndexRvalue) {
  using variant = rsl::variant<int, long>;
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(std::move(v)));
    ASSERT_SAME(decltype(get<0>(std::move(v))), int&&);
    ASSERT_EQ(get<0>(std::move(v)), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(get<1>(std::move(v))), long&&);
    ASSERT_EQ(get<1>(std::move(v)), 42l);
  }
  using variant_const = rsl::variant<const int, long>;
  {
    variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(std::move(v)));
    ASSERT_SAME(decltype(get<0>(std::move(v))), int&&);
    ASSERT_EQ(get<0>(std::move(v)), 42);
  }
  {
    variant_const v(42l);
    ASSERT_SAME(decltype(get<1>(std::move(v))), long&&);
    ASSERT_EQ(get<1>(std::move(v)), 42l);
  }
}

TEST(Get, IndexConstLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(v));
    ASSERT_SAME(decltype(get<0>(v)), const int&);
    static_assert(get<0>(v) == 42);
  }
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(v));
    ASSERT_SAME(decltype(get<0>(v)), const int&);
    ASSERT_EQ(get<0>(v), 42);
  }
  {
    constexpr variant v(42l);
    ASSERT_NOT_NOEXCEPT(get<1>(v));
    ASSERT_SAME(decltype(get<1>(v)), const long&);
    static_assert(get<1>(v) == 42);
  }
  {
    const variant v(42l);
    ASSERT_NOT_NOEXCEPT(get<1>(v));
    ASSERT_SAME(decltype(get<1>(v)), const long&);
    ASSERT_EQ(get<1>(v), 42);
  }

  using variant_const = rsl::variant<const int, long>;
  {
    constexpr variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(v));
    ASSERT_SAME(decltype(get<0>(v)), const int&);
    static_assert(get<0>(v) == 42);
  }
  {
    const variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(v));
    ASSERT_SAME(decltype(get<0>(v)), const int&);
    ASSERT_EQ(get<0>(v), 42);
  }
  {
    constexpr variant_const v(42l);
    ASSERT_NOT_NOEXCEPT(get<1>(v));
    ASSERT_SAME(decltype(get<1>(v)), const long&);
    static_assert(get<1>(v) == 42);
  }
  {
    const variant_const v(42l);
    ASSERT_NOT_NOEXCEPT(get<1>(v));
    ASSERT_SAME(decltype(get<1>(v)), const long&);
    ASSERT_EQ(get<1>(v), 42);
  }
}

TEST(Get, IndexConstRvalue) {
  using variant = rsl::variant<int, long>;
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(std::move(v)));
    ASSERT_SAME(decltype(get<0>(std::move(v))), const int&&);
    ASSERT_EQ(get<0>(std::move(v)), 42);
  }
  {
    const variant v(42l);
    ASSERT_SAME(decltype(get<1>(std::move(v))), const long&&);
    ASSERT_EQ(get<1>(std::move(v)), 42);
  }

  using variant_const = rsl::variant<const int, long>;
  {
    const variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<0>(std::move(v)));
    ASSERT_SAME(decltype(get<0>(std::move(v))), const int&&);
    ASSERT_EQ(get<0>(std::move(v)), 42);
  }
  {
    const variant_const v(42l);
    ASSERT_SAME(decltype(get<1>(std::move(v))), const long&&);
    ASSERT_EQ(get<1>(std::move(v)), 42);
  }
}

TEST(Get, IndexException) {
  // TODO
}

TEST(Get, TypeLvalue) {
  using variant = rsl::variant<int, long>;
  ASSERT_TRUE((std::same_as<rsl::variant_alternative<0, variant>::type, int>));
  ASSERT_TRUE((std::same_as<rsl::variant_alternative<1, variant>::type, long>));
  
  using variant_base = rsl::_variant_impl::variant_base<rsl::_impl::Storage<int, long>>;
  ASSERT_EQ((variant_base::alternatives.get_index_of(^^int)), 0);
  ASSERT_EQ((variant_base::alternatives.get_index_of(^^long)), 1);
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), int&);
    ASSERT_SAME(decltype(get<int&>(v)), int&);
    ASSERT_SAME(decltype(get<const int&>(v)), int&);
    ASSERT_SAME(decltype(get<int&&>(v)), int&);
    ASSERT_EQ(get<int>(v), 42);
    ASSERT_EQ(get<int&>(v), 42);
    ASSERT_EQ(get<const int&>(v), 42);
    ASSERT_EQ(get<int&&>(v), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(get<long>(v)), long&);
    ASSERT_SAME(decltype(get<const long&>(v)), long&);
    ASSERT_EQ(get<long>(v), 42);
    ASSERT_EQ(get<const long&>(v), 42);
  }

  using variant_const = rsl::variant<const int, long>;
  {
    variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), int&);
    ASSERT_SAME(decltype(get<int&>(v)), int&);
    ASSERT_SAME(decltype(get<const int&>(v)), int&);
    ASSERT_SAME(decltype(get<int&&>(v)), int&);
    ASSERT_EQ(get<int>(v), 42);
    ASSERT_EQ(get<int&>(v), 42);
    ASSERT_EQ(get<const int&>(v), 42);
    ASSERT_EQ(get<int&&>(v), 42);
  }
  {
    variant_const v(42l);
    ASSERT_SAME(decltype(get<long>(v)), long&);
    ASSERT_SAME(decltype(get<const long&>(v)), long&);
    ASSERT_EQ(get<long>(v), 42);
    ASSERT_EQ(get<const long&>(v), 42);
  }
}

TEST(Get, TypeRvalue) {
  using variant = rsl::variant<int, long>;
  {
    variant v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(std::move(v)));
    ASSERT_SAME(decltype(get<int>(std::move(v))), int&&);
    ASSERT_SAME(decltype(get<const int&>(std::move(v))), int&&);
    ASSERT_SAME(decltype(get<int&&>(std::move(v))), int&&);
    ASSERT_EQ(get<int>(std::move(v)), 42);
    ASSERT_EQ(get<const int&>(std::move(v)), 42);
    ASSERT_EQ(get<int&&>(std::move(v)), 42);
  }
  {
    variant v(42l);
    ASSERT_SAME(decltype(get<long>(std::move(v))), long&&);
    ASSERT_EQ(get<long>(std::move(v)), 42);
  }
}

TEST(Get, TypeConstRvalue) {
  using variant = rsl::variant<int, long>;
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(std::move(v)));
    ASSERT_SAME(decltype(get<int>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(get<const int&>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(get<const int&&>(std::move(v))), const int&&);
    ASSERT_EQ(get<int>(std::move(v)), 42);
    ASSERT_EQ(get<const int&>(std::move(v)), 42);
    ASSERT_EQ(get<const int&&>(std::move(v)), 42);
  }
  {
    const variant v(42l);
    ASSERT_SAME(decltype(get<long>(std::move(v))), const long&&);
    ASSERT_EQ(get<long>(std::move(v)), 42);
  }
  using variant_const = rsl::variant<const int, long>;
  {
    const variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(std::move(v)));
    ASSERT_SAME(decltype(get<int>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(get<const int&>(std::move(v))), const int&&);
    ASSERT_SAME(decltype(get<const int&&>(std::move(v))), const int&&);
    ASSERT_EQ(get<int>(std::move(v)), 42);
    ASSERT_EQ(get<const int&>(std::move(v)), 42);
    ASSERT_EQ(get<const int&&>(std::move(v)), 42);
  }
  {
    const variant_const v(42l);
    ASSERT_SAME(decltype(get<long>(std::move(v))), const long&&);
    ASSERT_EQ(get<long>(std::move(v)), 42);
  }
}

TEST(Get, TypeConstLvalue) {
  using variant = rsl::variant<int, long>;
  {
    constexpr variant v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), const int&);
    ASSERT_SAME(decltype(get<int&>(v)), const int&);
    ASSERT_SAME(decltype(get<const int&>(v)), const int&);
    ASSERT_SAME(decltype(get<int&&>(v)), const int&);
    static_assert(get<int>(v) == 42);
  }
  {
    const variant v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), const int&);
    ASSERT_EQ(get<int>(v), 42);
  }
  {
    constexpr variant v(42l);
    ASSERT_NOT_NOEXCEPT(get<long>(v));
    ASSERT_SAME(decltype(get<long>(v)), const long&);
    static_assert(get<long>(v) == 42);
  }
  {
    const variant v(42l);
    ASSERT_NOT_NOEXCEPT(get<long>(v));
    ASSERT_SAME(decltype(get<long>(v)), const long&);
    ASSERT_EQ(get<long>(v), 42);
  }
  using variant_const = rsl::variant<const int, long>;
  {
    constexpr variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), const int&);
    ASSERT_SAME(decltype(get<int&>(v)), const int&);
    ASSERT_SAME(decltype(get<const int&>(v)), const int&);
    ASSERT_SAME(decltype(get<int&&>(v)), const int&);
    static_assert(get<int>(v) == 42);
  }
  {
    const variant_const v(42);
    ASSERT_NOT_NOEXCEPT(get<int>(v));
    ASSERT_SAME(decltype(get<int>(v)), const int&);
    ASSERT_EQ(get<int>(v), 42);
  }
  {
    constexpr variant_const v(42l);
    ASSERT_NOT_NOEXCEPT(get<long>(v));
    ASSERT_SAME(decltype(get<long>(v)), const long&);
    static_assert(get<long>(v) == 42);
  }
  {
    const variant_const v(42l);
    ASSERT_NOT_NOEXCEPT(get<long>(v));
    ASSERT_SAME(decltype(get<long>(v)), const long&);
    ASSERT_EQ(get<long>(v), 42);
  }
}
