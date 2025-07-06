#include <rsl/constexpr_assert>

consteval bool foo(){
  auto x = 2;
  constexpr_assert(x > 10, "oh no");
  return false;
}

int main() {
  static constexpr auto f = foo();
}