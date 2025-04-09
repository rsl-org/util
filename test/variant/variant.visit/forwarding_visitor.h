#pragma once
#include <gtest/gtest.h>
#include <common/type_name.h>
#include <common/qualifiers.h>

template <typename T>
constexpr std::string_view hash_type() {
  return nameof<T>;
}

template <std::size_t N>
struct CallResult {
  using fptr_type = decltype(&hash_type<int>);

  Qualifiers qualifiers{};
  fptr_type args[N]{};

  template <typename... Args>
  constexpr explicit CallResult(Qualifiers this_qualifiers, Args&&... args_)
      : qualifiers{this_qualifiers}
      , args{std::forward<Args>(args_)...} {}

  template <typename... Args>
  constexpr void verify(Qualifiers expected_qualifiers) const {
    // qualifier mismatch
    ASSERT_EQ(qualifiers, expected_qualifiers) << "this qualifiers mismatched.\n"
                                               << "Expected: " << expected_qualifiers << '\n'
                                               << "Got:      " << qualifiers << '\n';

    // arity mismatch
    ASSERT_EQ(sizeof...(Args), N) << "Invalid arity. Expected " << sizeof...(Args) << " arguments, got " << N << ".\n";

    constexpr fptr_type expected_args[N]{hash_type<Args&&>...};
    for (std::size_t idx = 0; idx < N; ++idx) {
      // arg mismatch, call the functions to get actual types
      ASSERT_EQ(args[idx], expected_args[idx]) << "Argument at position " << idx << " mismatched.\n"
                                               << "Expected: " << expected_args[idx]() << '\n'
                                               << "Got:      " << args[idx]() << '\n';
    }
  }
};

template <typename... Ts>
CallResult(Qualifiers, Ts...) -> CallResult<sizeof...(Ts)>;

template <>
struct CallResult<0> {
  Qualifiers qualifiers{};

  template <typename... Args>
  void verify(Qualifiers expected_qualifiers) const {
    ASSERT_EQ(qualifiers, expected_qualifiers) << "this qualifiers mismatched.\n"
                                               << "Expected: " << expected_qualifiers << '\n'
                                               << "Got:      " << qualifiers << '\n';
  }
};

template <typename... Args>
constexpr auto make_call_result(Qualifiers object_qualifiers) {
  return CallResult{object_qualifiers, hash_type<Args&&>...};
}

struct ForwardingTestVisitor {
  template <typename... Args>
  constexpr auto operator()(Args&&...) & -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::CV::None, Qualifiers::Lvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) && -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::CV::None, Qualifiers::Rvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) const& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Const, Qualifiers::Lvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) const&& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Const, Qualifiers::Rvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) volatile& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Volatile, Qualifiers::Lvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) volatile&& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Volatile, Qualifiers::Rvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) const volatile& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Const | Qualifiers::Volatile, Qualifiers::Lvalue});
  }

  template <typename... Args>
  constexpr auto operator()(Args&&...) const volatile&& -> CallResult<sizeof...(Args)> {
    return make_call_result<Args&&...>({Qualifiers::Const | Qualifiers::Volatile, Qualifiers::Rvalue});
  }
};
