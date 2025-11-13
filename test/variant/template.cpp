#include <string>
#include <gtest/gtest.h>
#include <common/assertions.h>

#include <rsl/variant>
#include <rsl/string_view>

template <typename valueType, auto... Indices>
consteval std::array<valueType, sizeof...(Indices)> construct_array(
    std::integer_sequence<valueType, Indices...>) {
  return {(static_cast<valueType>(Indices))...};
};

enum class Options : uint8_t {
  option1,
  option2,
};
using variant_options = rsl::variant<int, rsl::string_view, Options>;
template <variant_options... Args>
struct WhoKnows {
  static constexpr size_t number_of_args = sizeof...(Args);

  template <typename T>
  static constexpr bool has = (rsl::holds_alternative<T>(Args) || ...);

  template <typename T>
  static constexpr bool hasOne = (static_cast<int>(rsl::holds_alternative<T>(Args)) + ...) == 1;

  template <typename T>
  constexpr T get() {
    static_assert(hasOne<T>);
    template for (constexpr auto index :
                  construct_array(std::integer_sequence<int, 0, number_of_args - 1>())) {
      if constexpr (rsl::holds_alternative<T>(Args...[index])) {
        return rsl::get<T>(Args...[index]);
      }
    }
    return rsl::get<T>(Args...[0]);
  }
};

TEST(Variant, MultipleParameters) {
  {
    WhoKnows<variant_options(42), variant_options(43)> obj;
    ASSERT_TRUE([:type_of(^^obj):] ::has<int>);
    ASSERT_FALSE([:type_of(^^obj):] ::hasOne<int>);
  }

  {
    WhoKnows<variant_options(42), variant_options(Options::option1)> obj;
    ASSERT_TRUE([:type_of(^^obj):] ::has<int>);
    ASSERT_TRUE([:type_of(^^obj):] ::hasOne<int>);
    ASSERT_EQ(obj.get<int>(), 42);
    ASSERT_NE(obj.get<int>(), 1);
  }

  {
    WhoKnows<variant_options(Options::option1)> obj;
    ASSERT_FALSE([:type_of(^^obj):] ::has<int>);
    ASSERT_TRUE([:type_of(^^obj):] ::has<Options>);

    ASSERT_FALSE([:type_of(^^obj):] ::hasOne<int>);
    ASSERT_TRUE([:type_of(^^obj):] ::hasOne<Options>);

    ASSERT_EQ(obj.get<Options>(), Options::option1);
    ASSERT_NE(obj.get<Options>(), Options::option2);
  }

  {
    WhoKnows<variant_options(Options::option1),
             variant_options(std::define_static_string("Hello world"))>
        obj;

    ASSERT_FALSE([:type_of(^^obj):] ::has<int>);
    ASSERT_TRUE([:type_of(^^obj):] ::has<Options>);
    ASSERT_TRUE([:type_of(^^obj):] ::has<rsl::string_view>);

    ASSERT_FALSE([:type_of(^^obj):] ::hasOne<int>);
    ASSERT_TRUE([:type_of(^^obj):] ::hasOne<Options>);
    ASSERT_TRUE([:type_of(^^obj):] ::hasOne<rsl::string_view>);

    ASSERT_EQ(obj.get<rsl::string_view>(), "Hello world");
  }
}
