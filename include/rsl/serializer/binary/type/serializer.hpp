#pragma once
#include <string>
#include <cstdint>
#include <meta>
#include <ranges>
#include <algorithm>
#include <bit>
#include <span>

#include <rsl/meta_traits>
#include "common.hpp"

namespace rsl::serializer::_binary_impl {
consteval uint8_t to_flags(std::meta::info type) {
    uint8_t value = 0;
    if (is_const(remove_reference(type))) {
        value |= Flags::CONST;
    }

    if (is_volatile(remove_reference(type))) {
        value |= Flags::VOLATILE;
    }

    if (is_pointer_type(remove_reference(type))) {
        value |= Flags::PTR;
    }

    if (is_reference_type(type)) {
        value |= Flags::LVREF;
    }

    if (meta::is_rvalue_reference(type)) {
        value |= Flags::RVREF;
    }
    return value;
}

consteval std::vector<uint8_t> flags_for(std::meta::info type) {
    uint8_t flags = to_flags(type);
    if (flags != 0) {
        std::vector flag_list = {flags};
        std::meta::info current = remove_reference(type);
        while (is_pointer_type((current))) {
            current = remove_pointer(current);
            flags = to_flags(current);
            if (flags == 0) {
                break;
            }
            flags |= 1U << 7U;
            flag_list.push_back(flags);
        }
        std::ranges::reverse(flag_list);
        return flag_list;
    }
    return {};
}

constexpr std::vector<uint8_t> serialize_string(std::string_view str) {
  // TODO encode size as varint
  uint8_t size = str.size() & 0xffU;
  std::vector<uint8_t> data{size};
  for (auto c : str) {
    data.push_back((uint8_t)c);
  }
  return data;
}

consteval std::meta::info base_type(std::meta::info type) {
  if (is_reference_type(type)) {
    type = remove_reference(type);
  }

  while (is_pointer_type(type)) {
    type = remove_pointer(type);
  }
  return remove_cvref(type);
}

class TypeSerializer {
  bool with_names = false;
  std::vector<uint8_t> data;

  consteval void serialize_uint(uint64_t value) {
    if (value < 0x7f) {
      data.push_back(value);
    }
    // todo varuint encoding
  }

  consteval void serialize_function(std::meta::info fnc) {
    // TODO lambdas?

    uint8_t value = static_cast<uint8_t>(ControlCode::function);
    value |= static_cast<uint8_t>(static_cast<uint8_t>(with_names) << 5U);
    data.push_back(value);

    if (with_names) {
      // TODO handle function template instances
      data.append_range(serialize_string(identifier_of(fnc)));
    }

    serialize(return_type_of(fnc));
    if (is_member_function_pointer_type(fnc)) {
      // value |= 1U << 4U;
      // TODO get class type
    } else if (meta::is_nonstatic_member_function(fnc)){
      value |= 1U << 4U;
      serialize(parent_of(fnc));
    }

    for (auto param : parameters_of(fnc)) {
      // TODO deal with optional params?
      serialize(param);
    }
  }

  consteval void serialize_array(std::meta::info type, bool fixed_size = true) {
    if (is_unbounded_array_type(type)) {
      fixed_size = false;
    }
    size_t rank = 1;
    auto element_type = type;
    if (is_array_type(type)) {
      rank = std::meta::rank(type);
      element_type = remove_all_extents(type);
    }
    
    uint8_t value = static_cast<uint8_t>(rank == 1 ? ControlCode::array : ControlCode::multi_array);
    value |= static_cast<uint8_t>(static_cast<uint8_t>(fixed_size) << 4U);
    data.push_back(value);

    if (fixed_size) {
      if (rank != 1) {
        serialize_uint(rank);
      }
      for (size_t idx = 0; idx < rank; ++idx) {
        serialize_uint(extent(type, idx));
      }
    }
    serialize(element_type);    
  }

  consteval uint8_t serialize_builtin(std::meta::info type) {
    uint8_t value             = 0;
    std::meta::info special[] = {^^bool, ^^char, ^^wchar_t, dealias(^^nullptr_t), ^^void};
    if (auto* it = std::ranges::find(special, type); it != std::end(special)) {
      value = 1U << 6U;
      value |= static_cast<uint8_t>(std::distance(std::begin(special), it));
    } else if (is_floating_point_type(type)) {
      value = 1U << 7U | 1U << 2U;
      value |= static_cast<uint8_t>(std::countr_zero(size_of(type) >> 2U));
    } else if (is_integral_type(type)) {
      value = 1U << 7U;
      if ((type != ^^char8_t)&&(type != ^^char16_t)&&(type != ^^char32_t)) {
        value |= 1U << 6U;
      } 
      if (is_signed_type(type)) {
        value |= 1U << 2U;
      }
      value |= static_cast<uint8_t>(std::countr_zero(size_of(type)));
    }
    return value;
  }

  consteval void serialize_aggregate(std::meta::info type) {
    uint8_t value = static_cast<uint8_t>(ControlCode::record);
    value |= (static_cast<unsigned>(with_names) << 5U);

    auto bases = bases_of(type, std::meta::access_context::current());
    if (!bases.empty()) {
      value |= 1U << 4U;
    }

    data.push_back(value);
    if (with_names) {
      // class name
      data.append_range(serialize_string(identifier_of(type)));
    }

    for (auto base : bases) {
      serialize(base, with_names);
    }

    if (!bases.empty()) {
      data.push_back(1U << 4U);
    }

    auto members = nonstatic_data_members_of(type, std::meta::access_context::current());
    for (auto member : members) {
      serialize(member, with_names);
    }
  }

  consteval void serialize(std::meta::info entity, bool with_names_ = true) {
    if (!with_names_) { with_names = false;  } // TODO

    auto type       = is_type(entity) ? entity : type_of(entity);
    auto underlying = base_type(type);
    bool is_named   = with_names && not is_type(entity) && has_identifier(entity);
    uint8_t flags   = to_flags(type);

    uint8_t value = 0;
    if (is_array_type(underlying)) {
      serialize_array(underlying);
    } else if (is_class_type(underlying) && is_aggregate_type(underlying)) {
      serialize_aggregate(underlying);
      data.push_back(static_cast<uint8_t>(is_named) << 5U);
    } else if (value = serialize_builtin(underlying); value != 0) {
      value |= static_cast<uint8_t>(static_cast<uint8_t>(flags != 0U) << 4U);
      value |= static_cast<uint8_t>(static_cast<uint8_t>(is_named) << 5U);
      data.push_back(value);
    } else {
      throw "unimplemented";
    }

    if (flags != 0) {
      data.append_range(flags_for(type));
    }

    if (is_named) {
      data.append_range(serialize_string(identifier_of(entity)));
    }
  }

public:
  consteval TypeSerializer() = default;

  static consteval std::span<uint8_t const> serialize_type(std::meta::info entity,
                                                           bool with_names = true) {
    auto serializer = TypeSerializer();
    serializer.serialize(entity, with_names);
    return define_static_array(serializer.data);
  }
};
}  // namespace rsl::serializer::_binary_impl