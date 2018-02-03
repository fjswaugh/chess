#pragma once

#include "chess/typedefs.h"

#include <cassert>
#include <iostream>

namespace Chess {

/**
 * A location on a chess board. The data is stored as a single integer that
 * ranges from 0 to 63.
 */
struct Location {
    constexpr Location(u8 data) : data_{data} {
        assert(data < 64);
    }
    constexpr operator u8() const { return data_; }

    constexpr Location(const char* str) : Location(str[0] - 'a', str[1] - '1') {}

    constexpr Location(int col, int row) : data_(col + row * 8) {
        assert(col >= 0 && col <= 7 && row >= 0 && row <= 7);
    }
    constexpr int row() const { return data_ / 8; }
    constexpr int col() const { return data_ % 8; }
private:
    u8 data_;
};

const char* to_string(Location);

constexpr bool operator==(Location a, Location b) {
    return u8(a) == u8(b);
}

constexpr bool operator!=(Location a, Location b) {
    return !(a == b);
}

}

