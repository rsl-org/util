#include <gtest/gtest.h>
#include <cstddef>

#include <rsl/variant>


template <class V, std::size_t E>
void check_size() {
  ASSERT_EQ(rsl::variant_size<V>::value, E);
  ASSERT_EQ(rsl::variant_size<V const>::value, E);

  ASSERT_EQ(rsl::variant_size_v<V>, E);
  ASSERT_EQ(rsl::variant_size_v<V const>, E);
}

TEST(Helpers, VariantSize) {
  check_size<rsl::variant<>, 0>();
  check_size<rsl::variant<int>, 1>();
  check_size<rsl::variant<int, char, void*>, 3>();
}
