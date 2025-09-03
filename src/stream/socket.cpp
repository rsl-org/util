#include <rsl/stream/socket.hpp>
#include <rsl/stream/stream.hpp>

#include <sys/socket.h>
#include <cerrno>

namespace rsl::stream {
IOResult SocketInputStream::try_read(std::byte* target, std::size_t size) {
  ssize_t n = ::recv(fd, target, size, MSG_DONTWAIT);
  if (n > 0) {
    return {static_cast<std::size_t>(n), IOStatus::ok};
  }
  if (n == 0) {
    return {0, IOStatus::end_of_stream};
  }
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return {0, IOStatus::would_block};
  }
  return {0, IOStatus::error};
}

IOResult SocketOutputStream::try_write(std::byte const* source, std::size_t size) {
  ssize_t n = ::send(fd, source, size, MSG_DONTWAIT);
  if (n >= 0) {
    return {static_cast<std::size_t>(n), IOStatus::ok};
  }
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return {0, IOStatus::would_block};
  }
  return {0, IOStatus::error};
}

}  // namespace rsl::stream
