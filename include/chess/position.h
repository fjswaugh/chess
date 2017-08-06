#pragma once

#include <cassert>
#include <string>
#include <stdexcept>

// Position is (at least for now) a thin wrapper over an integer that simply maintains the
// invariant that it is a valid position in the mailbox representation (that is its value is
// between 0 and 120).

struct Position {
    Position(const std::string& s) : Position(s.c_str()) {}
    Position(const char* const s) : Position(s[0] - 'A', s[1] - '1') {}
    Position(int col, int row)
        : value_{row * 10 + 20 + col + 1}
    {
        assert(col >= 0 && col <= 7 && row >= 0 && row <= 7 && "Position not on board");
    }
    Position(int i) : value_{i} {
        assert(i >= 0 && i < 120 && "Invalid position");
    }

    int col() const { return value_ % 10 - 1; }
    int row() const { return value_ / 10 - 2; }

    explicit operator int() const { return value_; }

    friend bool operator==(Position, Position);
    friend bool operator<(Position, Position);
    friend bool operator<=(Position, Position);
    friend Position operator+(Position, Position);
    friend Position operator-(Position, Position);
    friend Position operator+(Position, int);
    friend Position operator-(Position, int);
    // Pre-increment
    friend Position& operator++(Position&);
    friend Position& operator--(Position&);
private:
    // Value corresponds to the index of the mailbox
    int value_;
};

inline bool operator==(Position a, Position b) {
    return a.value_ == b.value_;
}

inline bool operator!=(Position a, Position b) {
    return !(a == b);
}

inline bool operator<(Position a, Position b) {
    return a.value_ < b.value_;
}

inline bool operator<=(Position a, Position b) {
    return a.value_ <= b.value_;
}

inline bool operator>(Position a, Position b) {
    return !(a <= b);
}

inline bool operator>=(Position a, Position b) {
    return !(a < b);
}

inline Position operator+(Position a, Position b) {
    return Position{a.value_ + b.value_};
}

inline Position operator-(Position a, Position b) {
    return Position{a.value_ - b.value_};
}

inline Position operator+(Position a, int b) {
    return Position{a.value_ + b};
}

inline Position operator-(Position a, int b) {
    return Position{a.value_ - b};
}

inline Position& operator+=(Position& p, int i) {
    return (p = p + i);
}

// Pre-increment
inline Position& operator++(Position& p) {
    assert(p.value_ < 120 - 1);
    p.value_++;
    return p;
}

inline Position& operator--(Position& p) {
    assert(p.value_ > 0);
    p.value_--;
    return p;
}

// Post-increment
inline Position operator++(Position& p, int) {
    const auto res = p;
    ++p;
    return res;
}

inline Position operator--(Position& p, int) {
    const auto res = p;
    --p;
    return res;
}

