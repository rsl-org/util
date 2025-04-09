#pragma once
#include <gtest/gtest.h>
#include <rsl/variant>

template <auto V>
struct Constant {
  static constexpr auto value = V;
};
