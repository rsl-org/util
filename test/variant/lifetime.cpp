#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility>
#include <variant>

#include <rsl/variant>
#include <common/lifetime.h>

template <typename T>
struct LifetimeTest : public testing::Test {
  using type = T;
};

template <template <typename...> class T, std::size_t... Idx>
auto get_test_type(std::index_sequence<Idx...>) {
  return std::type_identity<T<Lifetime<Idx>...>>{};
}
template <template <typename...> class T, std::size_t Count>
using test_type = typename decltype(get_test_type<T>(std::make_index_sequence<Count>()))::type;

union Bare {
  Lifetime<0> alt_0;
  Lifetime<1> alt_1;
  Lifetime<2> alt_2;
  Lifetime<3> alt_3;
  Lifetime<4> alt_4;

  Bare() {}
  ~Bare() {}
};

using test_types = testing::Types<test_type<std::variant, 5>,  // control
                                  test_type<rsl::variant, 5>>;
TYPED_TEST_SUITE(LifetimeTest, test_types);

TYPED_TEST(LifetimeTest, DefaultCtor) {
  LifetimeTracker::reset();
  { auto obj = TypeParam{}; }
  LifetimeTracker::assert_equal({{State::DefaultCtor, 0}, {State::Dtor, 0}});
}

TYPED_TEST(LifetimeTest, Ctor) {
  LifetimeTracker::reset();
  {
    auto obj = TypeParam{std::in_place_index<1>, 123};
    ASSERT_EQ(obj.index(), 1);
  }

  ASSERT_FALSE(LifetimeTracker::is_empty());
  LifetimeTracker::assert_equal({{State::Ctor, 1}, {State::Dtor, 1}});
}

TYPED_TEST(LifetimeTest, Copy) {
  {
    auto obj = TypeParam{std::in_place_index<1>, 123};
    ASSERT_EQ(obj.index(), 1);
    ASSERT_FALSE(LifetimeTracker::is_empty());
    LifetimeTracker::assert_equal({{State::Ctor, 1}});

    { auto obj2 = TypeParam{obj}; }
    LifetimeTracker::assert_equal({{State::CopyCtor, 1}, {State::Dtor, 1}});

    {
      TypeParam target{std::in_place_index<0>, 1};
      target = obj;
    }

    LifetimeTracker::assert_equal({{State::Ctor, 0}, {State::Dtor, 0}, {State::CopyCtor, 1}, {State::Dtor, 1}});
  }

  LifetimeTracker::assert_equal({{State::Dtor, 1}});
}

TYPED_TEST(LifetimeTest, MoveCtor) {
  {
    auto obj = TypeParam{std::in_place_index<1>, 123};
    ASSERT_EQ(obj.index(), 1);
    ASSERT_FALSE(LifetimeTracker::is_empty());

    LifetimeTracker::assert_equal({{State::Ctor, 1}});
    { auto obj2 = TypeParam{std::move(obj)}; }
    LifetimeTracker::assert_equal({{State::MoveCtor, 1}, {State::Dtor, 1}});
  }

  LifetimeTracker::assert_equal({{State::Dtor, 1}});
}

TYPED_TEST(LifetimeTest, MoveAssign) {
  {
    auto obj = TypeParam{std::in_place_index<1>, 123};
    ASSERT_EQ(obj.index(), 1);
    ASSERT_FALSE(LifetimeTracker::is_empty());
    LifetimeTracker::assert_equal({{State::Ctor, 1}});

    {
      TypeParam target{std::in_place_index<0>, 123};
      target = std::move(obj);
    }

    LifetimeTracker::assert_equal({{State::Ctor, 0}, {State::Dtor, 0}, {State::MoveCtor, 1}, {State::Dtor, 1}});
  }

  LifetimeTracker::assert_equal({{State::Dtor, 1}});
}

TYPED_TEST(LifetimeTest, Emplace) {
  LifetimeTracker::reset();
  {
    auto obj = TypeParam{std::in_place_index<1>, 123};
    ASSERT_EQ(obj.index(), 1);
    ASSERT_FALSE(LifetimeTracker::is_empty());
    LifetimeTracker::assert_equal({{State::Ctor, 1}});

    obj.template emplace<Lifetime<0>>(Lifetime<0>{0});
    LifetimeTracker::assert_equal({{State::Ctor, 0}, {State::Dtor, 1}, {State::MoveCtor, 0}, {State::Dtor, 0}});

    obj.template emplace<1>(1);
    LifetimeTracker::assert_equal({{State::Dtor, 0}, {State::Ctor, 1}});
  }
  LifetimeTracker::assert_equal({{State::Dtor, 1}});
}
