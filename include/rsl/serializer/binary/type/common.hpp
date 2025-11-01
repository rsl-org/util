#pragma once
#include <cstdint>

namespace rsl::serializer::_binary_impl {

enum Flags : uint8_t {
  HAS_MORE = 1U << 7U,
  PTR      = 1U << 4U,
  VOLATILE = 1U << 3U,
  CONST    = 1U << 2U,
  RVREF    = 1U << 1U | 1U << 0U,
  LVREF    = 1U << 0U,
};

enum class Category : uint8_t {
  control               = 0b00,
  special_types         = 0b01,
  character_or_floating = 0b10,
  integers              = 0b11
};

enum class ControlCode : uint8_t {
  record_end = 0b000,
  record     = 0b001,
};

}  // namespace rsl::serializer::_binary_impl