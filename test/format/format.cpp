#include <gtest/gtest.h>
#define RSL_INJECT_GLOBAL_FORMATTER
#include <rsl/format>

struct S {
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int g;
};

struct B {
  S i;
  S j;
};

struct BB {
  S i;
  S j;
};


struct I: BB, B, S {
    S Member;
};

template <>
struct std::formatter<B> : std::formatter<std::string_view> {
  auto format(B const& t, auto& ctx) const {
    return std::formatter<std::string_view>::format("B TYPE", ctx);
  }
};

TEST(Format, Format) {
  S s{1, 2, 3, 4, 5, 6, 7};
  auto formatted = rsl::format(s);
  ASSERT_EQ(formatted, "S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}");

  B b{
      {1, 2,  3,  4,  5,  6,  7},
      {8, 9, 10, 11, 12, 13, 14}
  };
  auto formattedB = rsl::format(b);
  ASSERT_EQ(formattedB,
            "B{.i=S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}, .j=S{.a=8, .b=9, .c=10, .d=11, .e=12, .f=13, .g=14}}");

  BB bb{
      {1, 2,  3,  4,  5,  6,  7},
      {8, 9, 10, 11, 12, 13, 14}
  };
  auto formattedBB = rsl::format(bb);
  ASSERT_EQ(formattedBB,
            "BB{.i=S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}, .j=S{.a=8, .b=9, .c=10, .d=11, .e=12, .f=13, .g=14}}");

  auto formattedI = rsl::format(I{bb, b, s, S{-1, -2}});
    ASSERT_EQ(formattedI,
                "I{BB{.i=S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}, .j=S{.a=8, .b=9, .c=10, .d=11, .e=12, .f=13, .g=14}}, B TYPE, S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}, .Member=S{.a=-1, .b=-2, .c=0, .d=0, .e=0, .f=0, .g=0}}");

}

TEST(StringView, formatter_injection){
    B b{{1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14}};
    auto formattedB = std::format("{}", b);
    ASSERT_EQ(formattedB,"B TYPE");

    BB bb{{1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14}};
    auto formattedBB = std::format("{}", bb);
    ASSERT_EQ(formattedBB,
              "BB{.i=S{.a=1, .b=2, .c=3, .d=4, .e=5, .f=6, .g=7}, .j=S{.a=8, .b=9, .c=10, .d=11, .e=12, .f=13, .g=14}}");
}
