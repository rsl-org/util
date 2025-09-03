#pragma once
#include "stream.hpp"

namespace rsl::stream {

struct SocketInputStream final : InputStream {
  explicit SocketInputStream(int fd) : fd(fd) {}
  ~SocketInputStream() override = default;
  IOResult try_read(std::byte* target, std::size_t size) override;
  IOResult read(std::byte* target, std::size_t size) override { return try_read(target, size); }

private:
  int fd;
};

struct SocketOutputStream final : OutputStream {
  explicit SocketOutputStream(int fd) : fd(fd) {}
  ~SocketOutputStream() override = default;

  IOResult try_write(std::byte const* source, std::size_t size) override;
  IOResult write(std::byte const* source, std::size_t size) override {
    return try_write(source, size);
  }

private:
  int fd;
};

}  // namespace rsl::stream
