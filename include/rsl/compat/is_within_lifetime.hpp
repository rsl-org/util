#pragma once
namespace rsl::compat {

#if __cpp_lib_within_lifetime < 202306L
template <class T>
constexpr bool is_within_lifetime(const T* ptr) noexcept {
  // introduced in P2641

  // this can be roughly emulated using __builtin_constant_p
  // note that this does not always work

#  if __has_builtin(__builtin_constant_p)
#    define HAS_IS_WITHIN_LIFETIME true
  return __builtin_constant_p(*ptr);
#  else
#    define HAS_IS_WITHIN_LIFETIME false
  static_assert(false, "is_within_lifetime not supported with this compiler.");
#  endif
}
#else
#  define HAS_IS_WITHIN_LIFETIME true
using std::is_within_lifetime;
#endif
}