#pragma once
#include <meta>

namespace rsl::serializer {

template <typename T>
concept complete_type = is_complete_type(^^T);

template <typename T>
concept incomplete_type = not complete_type<T>;

}