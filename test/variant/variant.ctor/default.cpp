#include <type_traits>
#include <gtest/gtest.h>

#include <rsl/variant>
#include <common/util.h>

struct NonDefaultConstructible {
  NonDefaultConstructible() = delete;
};

struct NotNoexcept {
  NotNoexcept() noexcept(false) {}
};

struct DefaultCtorThrows {
  DefaultCtorThrows() { throw 42; }
};

TEST(DefaultCtor, Constructible) {
  ASSERT_TRUE((std::is_default_constructible_v<rsl::variant<std::monostate>>));
  ASSERT_TRUE((std::is_default_constructible_v<rsl::variant<std::monostate, int>>));
  ASSERT_FALSE((std::is_default_constructible_v<rsl::variant<NonDefaultConstructible, int>>));
}

TEST(DefaultCtor, Noexcept) {
  ASSERT_TRUE((std::is_nothrow_default_constructible_v<rsl::variant<int>>));
  ASSERT_FALSE((std::is_nothrow_default_constructible_v<rsl::variant<NotNoexcept>>));
}
