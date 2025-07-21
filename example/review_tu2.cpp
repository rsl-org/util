#include <meta>
#define RSL_ENABLE_REVIEW
#include <rsl/assert>

namespace {
template <int A>
consteval void foo(int b) {
  constexpr_review(A != 42);
  constexpr_review(b != 42);
}

template <std::meta::info TArg>
consteval bool bar(std::meta::info arg) {
  constexpr_review(TArg == std::meta::info{});
  constexpr_review(arg == std::meta::info{});
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
  // constexpr_review(collection.size() == 0);
}

constexpr static auto C = 42;
}  // namespace

void run_tu2() {
  foo<42>(42);

  auto d = 42;
  constexpr_review(C != 42);
  constexpr_review(d != 42);

  // constexpr static auto _ = bar<^^::>(^^::);

  baz<^^::TestNS>();
}