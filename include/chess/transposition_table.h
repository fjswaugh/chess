#pragma once

#include "misc.h"
#include "move.h"

#include <array>

namespace Chess {

enum class Node_type : u8 {
    pv,
    fail_high,
    fail_low,
};

struct Transposition_node {
    u64 key = 0;
    Move best_move;
    u8 best_move_position = 0;
    i16 score = 0;
    u8 depth = 0;
    Node_type type = Node_type::pv;

    constexpr explicit operator bool() const {
        return key != 0;
    }
};

inline constexpr std::size_t index_of(u64 key) {
    // Take the lower 26 bits
    return key & 0b0000000000000000000000000000000000000011111111111111111111111111;
}

using Transposition_table = Transposition_node[1<<26];

}

