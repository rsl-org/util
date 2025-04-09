#pragma once
#include <experimental/meta>

template <typename T>
static constexpr char const* nameof = std::meta::define_static_string(display_string_of(^^T));