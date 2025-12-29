#pragma once
#include <rsl/macro>

#if $os_is(LINUX)
#  include <unistd.h>
#elif $os_is(WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#  ifndef STDIN_FILENO
#    define STDIN_FILENO _fileno(stdin)
#  endif
#  ifndef STDOUT_FILENO
#    define STDOUT_FILENO _fileno(stdout)
#  endif
#  ifndef STDERR_FILENO
#    define STDERR_FILENO _fileno(stderr)
#  endif
#else
#  error "Unsupported platform"
#endif

namespace rsl::_impl_platform {
inline bool isatty(int fd) {
#if $os_is(LINUX)
  return bool(::isatty(fd));
#elif $os_is(WINDOWS)
  return bool(::_isatty(fd));
#else
#  error "Unsupported platform"
#endif
}

}  // namespace rsl::_impl_platform