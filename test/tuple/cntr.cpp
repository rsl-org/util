#include <rsl/tuple>
#include <common/assertions.h>
#include <common/types.h>
#include <gtest/gtest.h>
#include <utility>

int DefaultOnly::count = 0;
TEST(Tuple, cntr_lazy_sfinae) {
  {
    NonConstCopyable v(42);
    BlowsUpOnConstCopy<int> b;
    rsl::tuple<NonConstCopyable, BlowsUpOnConstCopy<int>> t(v, b);
    ASSERT_EQ(rsl::get<0>(t).value, 42);
  }
}

template <std::size_t... I>
constexpr bool CreateTuple(rsl::index_sequence<I...>) {
  using LargeTuple  = rsl::tuple<std::integral_constant<std::size_t, I>...>;
  using TargetTuple = rsl::tuple<decltype(I)...>;
  LargeTuple tuple(std::integral_constant<std::size_t, I>{}...);
  static_assert(get<0>(tuple).value == 0);
  static_assert(get<sizeof...(I) - 1>(tuple).value == sizeof...(I) - 1);

  TargetTuple t1 = tuple;  // converting copy constructor from &
  TargetTuple t2 =
      static_cast<LargeTuple const&>(tuple);  // converting copy constructor from const&
  TargetTuple t3 = std::move(tuple);          // converting rvalue constructor
  TargetTuple t4 = static_cast<LargeTuple const&&>(tuple);  // converting const rvalue constructor
  TargetTuple t5;                                           // default constructor
  (void)t1;
  (void)t2;
  (void)t3;
  (void)t4;
  (void)t5;

  t1 = tuple;                                   // converting assignment from &
  t1 = static_cast<LargeTuple const&>(tuple);   // converting assignment from const&
  t1 = std::move(tuple);                        // converting assignment from &&
  t1 = static_cast<LargeTuple const&&>(tuple);  // converting assignment from const&&
  std::swap(t1, t2);                            // swap

  return true;
}

TEST(Tuple, cntr_recursion_depth) {
  static_assert(CreateTuple(rsl::make_index_sequence<512>{}));
}

template <class Elem>
void test_sfinae() {
  using Tup   = rsl::tuple<Elem>;
  using Alloc = std::allocator<int>;
  using Tag   = std::allocator_arg_t;
  // TODO : FIXME
  // special members
  {
    // static_assert(std::is_default_constructible<Tup>::value, "");
    // static_assert(std::is_move_constructible<Tup>::value, "");
    // static_assert(!std::is_copy_constructible<Tup>::value, "");
    // static_assert(!std::is_constructible<Tup, Tup&>::value, "");
  }
  // args constructors
  {
    // static_assert(std::is_constructible<Tup, Elem&&>::value, "");
    // static_assert(!std::is_constructible<Tup, Elem const&>::value, "");
    // static_assert(!std::is_constructible<Tup, Elem&>::value, "");
  }
  // // uses-allocator special member constructors
  // {
  //   static_assert(std::is_constructible<Tup, Tag, Alloc>::value, "");
  //   static_assert(std::is_constructible<Tup, Tag, Alloc, Tup&&>::value, "");
  //   static_assert(!std::is_constructible<Tup, Tag, Alloc, Tup const&>::value, "");
  //   static_assert(!std::is_constructible<Tup, Tag, Alloc, Tup&>::value, "");
  // }
  // // uses-allocator args constructors
  // {
  //   static_assert(std::is_constructible<Tup, Tag, Alloc, Elem&&>::value, "");
  //   static_assert(!std::is_constructible<Tup, Tag, Alloc, Elem const&>::value, "");
  //   static_assert(!std::is_constructible<Tup, Tag, Alloc, Elem&>::value, "");
  // }
}

TEST(Tuple, cntr_move) {
  {
    using T = rsl::tuple<>;
    T t0;
    T t = std::move(t0);
    ((void)t);  // Prevent unused warning
  }
  {
    using T = rsl::tuple<MoveOnly>;
    T t0(MoveOnly(0));
    T t = std::move(t0);
    ASSERT_EQ(get<0>(t).val, 0);
  }
  {
    using T = rsl::tuple<MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1));
    T t = std::move(t0);
    ASSERT_EQ(get<0>(t).val, 0);
    ASSERT_EQ(get<1>(t).val, 1);
  }
  {
    using T = rsl::tuple<MoveOnly, MoveOnly, MoveOnly>;
    T t0(MoveOnly(0), MoveOnly(1), MoveOnly(2));
    T t = std::move(t0);
    ASSERT_EQ(get<0>(t).val, 0);
    ASSERT_EQ(get<1>(t).val, 1);
    ASSERT_EQ(get<2>(t).val, 2);
  }
  {
    // TODO: FIXME
    // using d_t = rsl::tuple<ConstructsWithTupleLeaf>;
    // d_t d((ConstructsWithTupleLeaf()));
    // d_t d2(static_cast<d_t&&>(d));
  }
  {
    test_sfinae<move_only_ebo>();
    test_sfinae<move_only_large>();
  }
}

TEST(Tuple, cntr_trivially_constructible) {
  // TODO: Do we want to support this?
  // static_assert(std::is_trivially_constructible<rsl::tuple<>>::value, "");
}

TEST(Tuple, dtor_trivially_constructible) {
  static_assert(!std::is_trivially_destructible<TrackDtor>::value, "");

  static_assert(std::is_trivially_destructible<rsl::tuple<>>::value, "");
  static_assert(std::is_trivially_destructible<rsl::tuple<void*>>::value, "");
  static_assert(std::is_trivially_destructible<rsl::tuple<int, float>>::value, "");
  static_assert(!std::is_trivially_destructible<rsl::tuple<std::string>>::value, "");
  static_assert(!std::is_trivially_destructible<rsl::tuple<int, std::string>>::value, "");

  {
    int count = 0;
    {
      rsl::tuple<TrackDtor> tuple{TrackDtor(&count)};
      ASSERT_EQ(count, 0);
    }
    ASSERT_EQ(count, 1);
  }
}

TEST(Tuple, dtor_default) {
  {
    rsl::tuple<> t;
    (void)t;
  }
  {
    rsl::tuple<int> t;
    ASSERT_EQ(get<0>(t), 0);
  }
  {
    rsl::tuple<int, char*> t;
    ASSERT_EQ(get<0>(t), 0);
    ASSERT_EQ(get<1>(t), nullptr);
  }
  {
    rsl::tuple<int, char*, std::string> t;
    ASSERT_EQ(get<0>(t), 0);
    ASSERT_EQ(get<1>(t), nullptr);
    ASSERT_EQ(get<2>(t), "");
  }
  {
    rsl::tuple<int, char*, std::string, DefaultOnly> t;
    ASSERT_EQ(get<0>(t), 0);
    ASSERT_EQ(get<1>(t), nullptr);
    ASSERT_EQ(get<2>(t), "");
    ASSERT_EQ(get<3>(t), DefaultOnly());
  }
  {
    // See bug #21157.
    static_assert(!std::is_default_constructible<rsl::tuple<NoDefault>>(), "");
    static_assert(!std::is_default_constructible<rsl::tuple<DefaultOnly, NoDefault>>(), "");
    static_assert(!std::is_default_constructible<rsl::tuple<NoDefault, DefaultOnly, NoDefault>>(),
                  "");
  }
  {
    static_assert(noexcept(rsl::tuple<NoExceptDefault>()), "");
    static_assert(noexcept(rsl::tuple<NoExceptDefault, NoExceptDefault>()), "");

    static_assert(!noexcept(rsl::tuple<ThrowingDefault, NoExceptDefault>()), "");
    static_assert(!noexcept(rsl::tuple<NoExceptDefault, ThrowingDefault>()), "");
    static_assert(!noexcept(rsl::tuple<ThrowingDefault, ThrowingDefault>()), "");
  }
  {
    constexpr rsl::tuple<> t;
    (void)t;
  }
  {
    constexpr rsl::tuple<int> t{42};
    ASSERT_EQ(get<0>(t), 42);
  }
  {
    constexpr rsl::tuple<int> t;
    ASSERT_EQ(get<0>(t), 0);
  }
  {
    constexpr rsl::tuple<int, char*> t;
    ASSERT_EQ(get<0>(t), 0);
    ASSERT_EQ(get<1>(t), nullptr);
  }
  {
    // Check that the SFINAE on the default constructor is not evaluated when
    // it isn't needed. If the default constructor is evaluated then this test
    // should fail to compile.
    IllFormedDefault v(0);
    rsl::tuple<IllFormedDefault> t(v);
  }
  {
    struct Base {};
    struct Derived : Base {
    protected:
      Derived() = default;
    };
    static_assert(!std::is_default_constructible<rsl::tuple<Derived, int>>::value, "");
  }
}

TEST(Tuple, cntr_copy) {
    {
        using T = rsl::tuple<>;
        T t0;
        T t = t0;
        ((void)t); // Prevent unused warning
    }
    {
        using T = rsl::tuple<int>;
        T t0(2);
        T t = t0;
        assert(get<0>(t) == 2);
    }
    {
        using T = rsl::tuple<int, char>;
        T t0(2, 'a');
        T t = t0;
        assert(get<0>(t) == 2);
        assert(get<1>(t) == 'a');
    }
    {
        using T = rsl::tuple<int, char, std::string>;
        const T t0(2, 'a', "some text");
        T t = t0;
        assert(get<0>(t) == 2);
        assert(get<1>(t) == 'a');
        assert(get<2>(t) == "some text");
    }
    {
        using T = rsl::tuple<int>;
        constexpr T t0(2);
        constexpr T t = t0;
        static_assert(get<0>(t) == 2, "");
    }
    {
        using T = rsl::tuple<Empty>;
        constexpr T t0;
        constexpr T t = t0;
        constexpr Empty e = get<0>(t);
        ((void)e); // Prevent unused warning
    }


}


TEST(Tuple, cntr_convert_non_const_copy) {
  // test implicit conversions.
  // sizeof...(Types) == 1
  // {
  //   rsl::tuple<MutableCopy> t1{1};
  //   rsl::tuple<ConvertibleFrom<MutableCopy>> t2 = t1;
  //   assert(get<0>(t2).v.val == 1);
  // }

  // test implicit conversions.
  // sizeof...(Types) > 1
  {
    rsl::tuple<MutableCopy, int> t1{1, 2};
    rsl::tuple<ConvertibleFrom<MutableCopy>, int> t2{t1};
    rsl::tuple<ConvertibleFrom<MutableCopy>, int> t3 = t1;
    ASSERT_EQ(get<0>(t2).v.val , 1);
    ASSERT_EQ(get<1>(t2) , 2);
  }

  // // test explicit conversions.
  // // sizeof...(Types) == 1
  // // {
  // //   rsl::tuple<MutableCopy> t1{1};
  // //   rsl::tuple<ExplicitConstructibleFrom<MutableCopy>> t2{t1};
  // //   assert(get<0>(t2).v.val == 1);
  // // }

  // test explicit conversions.
  // sizeof...(Types) > 1
  {
    rsl::tuple<MutableCopy, int> t1{1, 2};
    rsl::tuple<ExplicitConstructibleFrom<MutableCopy>, int> t2{t1};
    ASSERT_EQ(get<0>(t2).v.val ,1);
    ASSERT_EQ(get<1>(t2) ,2);
  }

  // // test constraints

  // sizeof...(Types) != sizeof...(UTypes)
  static_assert(!std::is_constructible_v<rsl::tuple<int, int>, rsl::tuple<int>&>);
  static_assert(!std::is_constructible_v<rsl::tuple<int>, rsl::tuple<int, int>&>);
  static_assert(!std::is_constructible_v<rsl::tuple<int, int, int>, rsl::tuple<int, int>&>);

  // !(is_constructible_v<Types, decltype(get<I>(FWD(u)))> && ...)
  static_assert(!std::is_constructible_v<rsl::tuple<int, NoConstructorFromInt>, rsl::tuple<int, int>&>);

  // // sizeof...(Types) == 1 && other branch of "||" satisfied
  // {
  //   rsl::tuple<TracedCopyMove> t1{};
  //   rsl::tuple<ConvertibleFrom<TracedCopyMove>> t2{t1};
  //   assert(nonConstCopyCtrCalled(get<0>(t2).v));
  // }

  // // sizeof...(Types) == 1 && is_same_v<T, U>
  // {
  //   rsl::tuple<TracedCopyMove> t1{};
  //   rsl::tuple<TracedCopyMove> t2{t1};
  //   assert(!nonConstCopyCtrCalled(get<0>(t2)));
  // }

  // sizeof...(Types) != 1
  {
    rsl::tuple<TracedCopyMove, TracedCopyMove> t1{};
    rsl::tuple<TracedCopyMove, TracedCopyMove> t2{t1};
    ASSERT_TRUE(nonConstCopyCtrCalled(get<0>(t2)));
  }
}
