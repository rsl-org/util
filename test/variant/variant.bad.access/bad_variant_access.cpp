#include <type_traits>
#include <exception>
#include <variant>

#include <gtest/gtest.h>
#include <rsl/variant>

TEST(Exception, BadVariantAccess) {
  ASSERT_TRUE((std::is_base_of_v<std::exception, rsl::bad_variant_access>));
  ASSERT_TRUE(noexcept(std::bad_variant_access{})) << "bad_variant_access default ctor must be noexcept";
  ASSERT_TRUE(noexcept(std::bad_variant_access{}.what())) << "bad_variant_access::what() must be noexcept";

  std::bad_variant_access exception;
  ASSERT_NE(exception.what(), nullptr) << "A message must be returned";
}