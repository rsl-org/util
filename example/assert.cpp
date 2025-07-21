#include <meta>
#include <rsl/assert>

template <int A>
consteval void foo(int b) {
  constexpr_assert(A == 42);
  constexpr_assert(b == 42);
}

template <std::meta::info TArg>
consteval bool bar(std::meta::info arg) {
  constexpr_assert(TArg != std::meta::info{});
  constexpr_assert(arg != std::meta::info{});
  return true;
}

namespace TestNS {
inline int zoinks() {
  return 3;
}
inline bool boings() {
  return false;
}
}  // namespace TestNS

template <std::meta::info TArg>
void baz() {
  static constexpr auto collection =
      define_static_array(members_of(TArg, std::meta::access_context::current()));
  constexpr_assert(collection.size() != 0);
}

constexpr static auto C = 42;
int main() {
  foo<42>(42);

  auto d = 42;
  constexpr_assert(C == 42);
  constexpr_assert(d == 42);

  constexpr static auto _ = bar<^^::>(^^::);

  baz<^^::TestNS>();
}