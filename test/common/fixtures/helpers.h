#pragma once
#include <gtest/gtest.h>
#include <common/util.h>

template <typename T>
struct Helpers : testing::Test {};
TYPED_TEST_SUITE(Helpers, test_variants);