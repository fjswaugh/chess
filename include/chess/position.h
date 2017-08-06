#pragma once

#include <cassert>
#include <string>
#include <stdexcept>

struct Position {
    Position(const std::string& s) : Position(s.c_str()) {}
    Position(const char* const s) : Position(s[0] - 'A', s[1] - '1') {}
    explicit Position(int i) : Position(i % 10 - 1, i / 10 - 2) {}
    Position(int col, int row)
        : value_{row * 10 + 20 + col + 1}
    {
        // Position must be on the board
        assert(col >= 0 && col <= 7 && row >= 0 && row <= 7);
    }

    int col() const { return value_ % 10 - 1; }
    int row() const { return value_ / 10 - 2; }

    int value() const { return value_; }
private:
    // Value corresponds to the index of the mailbox
    int value_;
};

inline bool operator==(const Position a, const Position b)
{
    return a.value() == b.value();
}

