#pragma once
#include <cstddef>
#include <type_traits>

namespace rsl::stream {

enum class IOStatus : unsigned char {
  ok,
  error,
  would_block,
  out_of_space,
  end_of_stream,
  unsupported,
  decode_error
};

struct IOResult {
  std::size_t count;
  IOStatus status;
};

struct StreamBase {
  template <typename S, typename BufferPtr>
  IOResult blocking_loop(this S&& self,
                         IOResult (std::remove_cvref_t<S>::*try_fn)(BufferPtr, std::size_t),
                         BufferPtr buffer,
                         std::size_t size) {
    std::size_t total = 0;
    while (total < size) {
      IOResult r = (self.*try_fn)(buffer + total, size - total);
      total += r.count;

      switch (r.status) {
        case IOStatus::ok:
          if (r.count == 0) {
            return {total, IOStatus::error};
          }
          break;
        case IOStatus::would_block: continue;
        default: return {total, r.status};
      }
    }
    return {total, IOStatus::ok};
  }
};

struct InputStream : virtual StreamBase {
  virtual ~InputStream()                                         = default;
  virtual IOResult try_read(std::byte* target, std::size_t size) = 0;
  virtual IOResult read(std::byte* target, std::size_t size) {
    return blocking_loop(&InputStream::try_read, target, size);
  }
};

struct OutputStream : virtual StreamBase {
  virtual ~OutputStream()                                               = default;
  virtual IOResult try_write(std::byte const* source, std::size_t size) = 0;
  virtual IOResult write(std::byte const* source, std::size_t size) {
    return blocking_loop(&OutputStream::try_write, source, size);
  }
  virtual void reserve(std::size_t size) {}
};

}  // namespace rsl::stream