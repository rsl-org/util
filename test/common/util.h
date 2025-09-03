#pragma once
#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <cstddef>


template <auto V>
struct Constant {
  static constexpr auto value = V;
};

namespace test_util {
  inline std::vector<std::byte> to_bytes(std::string const& s) {
    std::vector<std::byte> out;
    out.reserve(s.size());
    for (unsigned char c : s) {
      out.push_back(static_cast<std::byte>(c));
    }
    return out;
  }

  inline std::vector<std::byte> bytes(auto... vals) {
    return {{std::byte(vals)...}};
  }
}