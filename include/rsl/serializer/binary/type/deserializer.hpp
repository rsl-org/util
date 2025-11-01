#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <span>

#include <unordered_map>

#include "common.hpp"

namespace rsl::serializer::_binary_impl {
constexpr std::string stringify_flags(uint8_t flags) {
  std::string out;

  if ((flags >> 4U) & 1) {
    out += "*";
  }
  if ((flags >> 3U) & 1) {
    out += " volatile";
  }
  if ((flags >> 2U) & 1) {
    out += " const";
  }
  if ((flags >> 1U) & 1) {
    out += "&";
  }
  if (flags & 1) {
    out += "&";
  }
  return out;
}

constexpr std::tuple<size_t, std::string> deserialize_string(std::span<uint8_t const> const& data) {
  uint8_t size = data[0];

  std::string name;
  name.reserve(size);
  for (size_t idx = 1; idx <= size; ++idx) {
    name += data[idx];
  }
  return {size + 1, name};
}

class Parser {
  std::span<uint8_t const> data;
  size_t cursor = 0;

  size_t nesting_level = 0;
  std::string out;

public:
  std::unordered_map<std::string, std::string> records;
  explicit Parser(std::span<uint8_t const> data) : data(data) {}

  std::string parse_string() {
    uint8_t size = data[cursor];

    std::string name;
    name.reserve(size);
    for (auto start = cursor + 1; cursor <= start + size; ++cursor) {
      name += data[cursor];
    }
    cursor += size;
    return name;
  }

  static std::string parse_builtin(uint8_t op) {
    std::string ret;
    auto category = Category(op >> 6U);
    switch (category) {
      using enum Category;
      case control:  //  not a builtin
        break;
      case special_types: {
        auto idx              = op & 0b111U;
        auto special = std::to_array<std::string_view>({"bool", "char", "wchar_t", "nullptr_t", "void"});
        ret += special.at(idx);
      } break;
      case character_or_floating: {
        bool is_float = bool(static_cast<uint8_t>(op >> 2U) & 1U);
        if (is_float) {
          auto size = (1U << (op & 0b11U)) << 2U;
          auto names = std::to_array<std::string_view>({"float", "double", "long double"});
          ret += names.at(op & 0b11U);
        } else {
          // character type
          auto size = 1U << (op & 0b11U);
          ret += "char";
          ret += std::to_string(size * 8);
          ret += "_t";
        }
      } break;
      case integers: {
        bool sign = bool(static_cast<uint8_t>(op >> 2U) & 1U);
        auto size = 1U << (op & 0b11U);

        if (sign) {
          ret += "u";
        }
        ret += "int";
        ret += std::to_string(size * 8);
      } break;
    }
    return ret;
  }

  void parse_struct() {
    bool is_named  = bool(static_cast<uint8_t>(data[cursor] >> 5U) & 1U);
    bool has_flags = bool(static_cast<uint8_t>(data[cursor] >> 4U) & 1U);

    out += "struct ";
    if (is_named) {
      auto [offset, name] = deserialize_string(data.subspan(cursor + 1));
      cursor += offset;
      out += name;
      out += " ";
    }
    out += "{\n";
    if (has_flags) {  // has bases
      do {
        parse_field();
      } while (data[cursor] != (1U << 4U));
    }
  }

  void parse_field() {
    auto category = Category(data[cursor] >> 6U);
    if (category == Category::control) {
      // control

      auto code = ControlCode(data[cursor] & 0b111U);
      switch (code) {
        using enum ControlCode;
        case record_end:
          --nesting_level;
          out += std::string(nesting_level * 2, ' ');
          out += "}";
          break;
        case record:
          out += std::string(nesting_level * 2, ' ');
          ++nesting_level;
          parse_struct();
          break;
      }
    } else {
      // builtin type
      out += parse_builtin(data[cursor]);
    }

    bool is_named  = bool(static_cast<uint8_t>(data[cursor] >> 5U) & 1U);
    bool has_flags = bool(static_cast<uint8_t>(data[cursor] >> 4U) & 1U);
    if (has_flags) {
      uint8_t flags = 0;
      do {
        flags = data[++cursor];
        out += stringify_flags(flags);
      } while (bool(static_cast<uint8_t>(flags >> 7U) & 1U));
    }

    if (is_named) {
      auto [offset, name] = deserialize_string(data.subspan(cursor + 1));
      cursor += offset;
      out += " ";
      out += name;
    }
    out += ";\n";
  }

  std::string parse() {
    /*for (cursor = 0; cursor < data.size(); ++cursor) {
      uint8_t op = data[cursor];
      auto category = op >> 6;  // top 2 bits
      bool is_named = (op >> 5U) & 1;
      bool has_flags = (op >> 4U) & 1;

      if (category == 0) {
        // control
        auto code = (op & 0b111);
        if (code == 1) {
          out += std::string(nesting_level * 2, ' ');
          ++nesting_level;
          out += "struct ";
          if (is_named) {
            auto [offset, name] = deserialize_string(data.subspan(cursor + 1));
            cursor += offset;
            out += name;
            out += " ";
          }
          out += "{\n";
          if (has_flags) {  // has bases
          }
        } else if (code == 0) {
          if (has_flags) {  // has bases
            continue;
          }
          --nesting_level;
          out += std::string(nesting_level * 2, ' ');
          out += "}";
          // check flags as well
          if (is_named) {
            auto [offset, name] = deserialize_string(data.subspan(cursor + 1));
            cursor += offset;
            out += " ";
            out += name;
          }
          out += ";\n";
        }
        continue;
      }

      out += std::string(nesting_level * 2, ' ');
      out += parse_builtin(op);

      if (has_flags) {
        uint8_t flags = 0;
        do {
          flags = data[++cursor];
          out += stringify_flags(flags);
        } while ((flags >> 7U) & 1);
      }

      if (is_named) {
        auto [offset, name] = deserialize_string(data.subspan(cursor + 1));
        cursor += offset;
        out += " ";
        out += name;
      }
      out += ";\n";
    }*/
    while (cursor < data.size()) {
      parse_field();
    }
    return out;
  }
};
}  // namespace rsl::serializer::_binary_impl