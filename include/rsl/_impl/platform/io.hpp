#pragma once

#ifdef __unix__
#  include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
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
#if __unix__
  return bool(::isatty(fd));
#else
  return bool(::_isatty(fd));
#endif
}

}  // namespace rsl::_impl_platform