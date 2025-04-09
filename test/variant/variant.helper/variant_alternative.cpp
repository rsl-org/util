#include <gtest/gtest.h>
#include <cstddef>

#include <rsl/variant>
#include <common/assertions.h>

template <class Variant, std::size_t Idx, class Expected>
void check_alternatives() {
  // type
  ASSERT_SAME((typename rsl::variant_alternative<Idx, Variant>::type), Expected);
  ASSERT_SAME((typename rsl::variant_alternative<Idx, const Variant>::type), const Expected);

  // alias
  ASSERT_SAME((rsl::variant_alternative_t<Idx, Variant>), Expected);
  ASSERT_SAME((rsl::variant_alternative_t<Idx, const Variant>), const Expected);
}

union AlternativeUnion {
  int member_0;
  void* member_1;
  const void* member_2;
  long double member_3;
};

TEST(Helpers, VariantAlternative) {
  using variant_t = rsl::variant<int, void*, const void*, long double>;
  check_alternatives<variant_t, 0, int>();
  check_alternatives<variant_t, 1, void*>();
  check_alternatives<variant_t, 2, const void*>();
  check_alternatives<variant_t, 3, long double>();
}

// todo ensure out of bounds access causes a static assertion failure