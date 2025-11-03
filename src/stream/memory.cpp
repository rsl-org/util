#include <rsl/stream/memory.hpp>
#include <rsl/stream/stream.hpp>

namespace rsl::stream {
IOResult MemoryInputStream::try_read(std::byte* target, std::size_t size) {
  std::size_t avail = buf_.size() - cursor;
  std::size_t n     = std::min(avail, size);
  if (n == 0) {
    return {0, IOStatus::end_of_stream};
  }
  std::copy_n(buf_.data() + cursor, n, target);
  cursor += n;
  return {n, IOStatus::ok};
}

IOResult VectorOutputStream::try_write(std::byte const* source, std::size_t size) {
  try {
    buffer.append_range(std::span(source, size));
    return {size, IOStatus::ok};
  } catch (...) { return {0, IOStatus::error}; }
}

void VectorOutputStream::reserve(std::size_t size) {
  buffer.reserve(size);
}
}  // namespace rsl::stream
