#pragma once
#include <span>
#include <vector>

#include "stream.hpp"

namespace rsl::stream {

struct VectorOutputStream final : OutputStream {
  ~VectorOutputStream() override = default;

  IOResult try_write(std::byte const* source, std::size_t size) override;
  IOResult write(std::byte const* source, std::size_t size) override {
    return try_write(source, size);
  }
  void reserve(std::size_t size) override;

  [[nodiscard]] std::vector<std::byte>& data() { return buffer; }
  [[nodiscard]] std::vector<std::byte> const& data() const { return buffer; }

private:
  std::vector<std::byte> buffer;
  friend struct MemoryInputStream;
};

struct MemoryInputStream final : InputStream {
  explicit MemoryInputStream(VectorOutputStream const& stream) : buf_(stream.buffer.begin(), stream.buffer.end()) {}
  explicit MemoryInputStream(std::span<std::byte const> data) : buf_(data) {}
  ~MemoryInputStream() override = default;
  IOResult try_read(std::byte* target, std::size_t size) override;
  IOResult read(std::byte* target, std::size_t size) override { return try_read(target, size); }

private:
  std::span<std::byte const> buf_;
  std::size_t cursor{0};
};

}  // namespace rsl::stream
