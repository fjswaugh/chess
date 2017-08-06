#pragma once

#include <string>

#include "position.h"

struct Move {
    Position begin;
    Position end;

    static Move from_string(const std::string& s) {
        if (s.length() < 5) throw std::runtime_error("Cannot construct move");

        auto begin = Position{s[0]-'a', s[1]-'1'};
        auto end   = Position{s[3]-'a', s[4]-'1'};
        return Move{begin, end};
    }
};

inline bool operator==(const Move& a, const Move& b)
{
    return a.begin == b.begin && a.end == b.end;
}

inline std::string to_string(Move move)
{
    std::string s;
    s.push_back(move.begin.col() + 'a');
    s.push_back(move.begin.row() + '1');
    s.push_back('-');
    s.push_back(move.end.col() + 'a');
    s.push_back(move.end.row() + '1');

    return s;
}

