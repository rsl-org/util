#include <string>
#include <gtest/gtest.h>
#include <common/assertions.h>

#include <rsl/variant>
#include <rsl/tagged_variant>

enum class Foo {
  foo [[=rsl::type<int>]],
  bar [[=rsl::type<char>]]
};

using variant_swap_p = ::testing::Types<rsl::variant<int,float>, rsl::tagged_variant<Foo>>;
template<typename T>
struct VariantSwap : public testing::Test
{
    using type = T;
};
TYPED_TEST_SUITE(VariantSwap, variant_swap_p);

TYPED_TEST(VariantSwap, SameIndex){
  using variant = rsl::variant<int, float>;

  {
    auto obj_1 = variant(1);
    auto obj_2 = variant(420);

    ASSERT_EQ(obj_1.index(), 0);
    ASSERT_EQ(obj_2.index(), 0);
    ASSERT_EQ(rsl::get<0>(obj_1), 1);
    ASSERT_EQ(rsl::get<0>(obj_2), 420);

    swap(obj_1, obj_2);

    ASSERT_EQ(obj_1.index(), 0);
    ASSERT_EQ(obj_2.index(), 0);
    ASSERT_EQ(rsl::get<0>(obj_1), 420);
    ASSERT_EQ(rsl::get<0>(obj_2), 1);
  }
}

TYPED_TEST(VariantSwap, DifferentIndex){
  using variant = rsl::variant<int, float, std::string>;

  {
    auto obj_1 = variant(1);
    auto obj_2 = variant("foo");

    ASSERT_EQ(obj_1.index(), 0);
    ASSERT_EQ(obj_2.index(), 2);
    swap(obj_1, obj_2);

    ASSERT_EQ(obj_1.index(), 2);
    ASSERT_EQ(obj_2.index(), 0);
  }
}
