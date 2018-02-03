#pragma once

#include "misc.h"
#include "move.h"

#include <vector>

namespace Chess {

enum class Node_type : u8 {
    pv,
    fail_high,
    fail_low,
};

/**
 * A node contains information on a position that may be useful in analysis.
 */
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

static_assert(sizeof(Transposition_node) == 16);

/**
 * A transposition table stores information on positions already seen in the
 * search tree. It is effectively a hash map, using the Zobrist hash function.
 */
struct Transposition_table {
    Transposition_table(u8 log_size = 26)
        : nodes_(1 << log_size), mask_{0}
    {
        for (u8 i = 0; i < log_size; ++i) {
            mask_ |= (1 << i);
        }
    }

    Transposition_node& at(u64 key) {
        return nodes_[this->index_of(key)];
    }

    const Transposition_node& at(u64 key) const {
        return nodes_[this->index_of(key)];
    }
private:
    constexpr std::size_t index_of(u64 key) const {
        return key & mask_;
    }

    std::vector<Transposition_node> nodes_;
    u64 mask_;
};
}

