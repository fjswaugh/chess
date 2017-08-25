#pragma once

#include <cstdint>

namespace Chess {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

constexpr u8  operator""_u8 (unsigned long long int i) { return i; }
constexpr u16 operator""_u16(unsigned long long int i) { return i; }
constexpr u32 operator""_u32(unsigned long long int i) { return i; }
constexpr u64 operator""_u64(unsigned long long int i) { return i; }

}
