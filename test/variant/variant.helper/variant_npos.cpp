#include <gtest/gtest.h>
#include <cstddef>

#include <rsl/variant>

TEST(Helpers, VariantNpos) {
  ASSERT_EQ(rsl::variant_npos, static_cast<std::size_t>(-1ULL));
}
