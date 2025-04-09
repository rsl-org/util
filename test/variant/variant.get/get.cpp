#include <gtest/gtest.h>
#include <rsl/variant>

#include <common/assertions.h>

TEST(Get, IndexLvalue){
  using variant = rsl::variant<const int, float>;

  {
    constexpr variant obj(42);
    ASSERT_NOT_NOEXCEPT(rsl::get<0>(obj));
    ASSERT_SAME(decltype(rsl::get<0>(obj)), const int&);
    static_assert(rsl::get<0>(obj) == 42, "Value mismatch");
  }
}
TEST(Get, IndexRvalue){
  // TODO
}

TEST(Get, IndexConstLvalue){
  // TODO
}

TEST(Get, IndexConstRvalue){
  // TODO
}

TEST(Get, IndexException){
  // TODO
}

TEST(Get, TypeLvalue){
  // TODO
}

TEST(Get, TypeRvalue){
  // TODO
}

TEST(Get, TypeConstLvalue){
  // TODO
}

TEST(Get, TypeConstRvalue){
  // TODO
}
