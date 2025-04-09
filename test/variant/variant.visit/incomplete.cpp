#include <gtest/gtest.h>
#include <rsl/variant>


struct Incomplete;
template <typename T>
struct Wrapper { T obj; };

TEST(Visit, Incomplete){
  //! TODO

  // auto obj = rsl::variant<Wrapper<Incomplete>*, int>{};

  // rsl::visit([](auto){}, obj);
  // rsl::visit([](auto) -> Wrapper<Incomplete>* { return nullptr; }, obj);
  // rsl::visit<void>([](auto){}, obj);
  // rsl::visit<void*>([](auto) -> Wrapper<Incomplete>* { return nullptr; }, obj);
}