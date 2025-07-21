#include <meta>
#include <print>
#define RSL_ENABLE_REVIEW
#include <rsl/assert>

namespace TestNS {
  inline void foo() {}
  inline bool bar() { return true; }
}

consteval void zoink() {
  static constexpr auto members =
      define_static_array(members_of(^^TestNS, std::meta::access_context::current()));
  constexpr_review(members.size() == 42, "foo");
  constexpr_review(members.size() == 42, "foo");
}

template <int A>
consteval void bar() {
  constexpr_review(A == 42);
}

void run_tu2();
int main() {
  run_tu2();
  zoink();
  bar<10>();
  bar<42>();
  constexpr_review(42, "foo");
  std::println("count: {}",rsl::_assert_impl::observations().size());
  for (auto assertion: rsl::_assert_impl::observations()) {
    std::println("{}", assertion);
  }
}