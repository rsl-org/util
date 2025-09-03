#pragma once
#include <span>
#include <vector>

#include "stream.hpp"

namespace rsl::stream {

struct MemoryInputStream final : InputStream {
  explicit MemoryInputStream(std::span<std::byte> data) : buf_(data) {}
  ~MemoryInputStream() override = default;
  IOResult try_read(std::byte* target, std::size_t size) override;
  IOResult read(std::byte* target, std::size_t size) override { return try_read(target, size); }

private:
  std::span<std::byte> buf_;
  std::size_t cursor{0};
};

struct VectorOutputStream final : OutputStream {
  explicit VectorOutputStream(std::vector<std::byte>& data) : buffer(&data) {}
  ~VectorOutputStream() override = default;

  IOResult try_write(std::byte const* source, std::size_t size) override;
  IOResult write(std::byte const* source, std::size_t size) override {
    return try_write(source, size);
  }
  void reserve(std::size_t size) override;

private:
  std::vector<std::byte>* buffer;
};

}  // namespace rsl::stream
