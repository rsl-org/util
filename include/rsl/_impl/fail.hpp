#pragma once
#include <experimental/meta>
#include <string_view>


namespace rsl::_impl {

namespace _fail {
template <char const* message>
consteval void fail() {
    static_assert(false, std::string_view{message});
}
}

consteval void fail(std::string_view message) {
    using std::meta::reflect_constant;
    extract<void(*)()>(substitute(^^_fail::fail, {reflect_constant(define_static_string(message))}))();
}
}