#pragma once

#include "chess/misc.h"
#include "chess/square.h"

#include <string>
#include <array>
#include <cstring>

namespace Chess {

struct Position {
    Position(const std::string& s)
        : Position(s.c_str())
    {}
    Position(const char* const s)
        : Position(s[0] - 'a', s[1] - '1')
    {
        if (std::strlen(s) != 2) {
            throw std::invalid_argument("Cannot construct position from string where length != 2");
        }
    }
    Position(unsigned col, unsigned row)
        : col_{col}, row_{row}
    {
        if (col > 7 || row > 7) {
            throw std::invalid_argument("Position not on board");
        }
    }
    unsigned col() const { return col_; }
    unsigned row() const { return row_; }
private:
    unsigned col_;
    unsigned row_;
};

inline bool operator==(Position a, Position b) {
    return a.col() == b.col() && a.row() == b.row();
}

struct Move {
    Position begin;
    Position end;

    static Move from_string(const std::string& s) {
        if (s.length() != 5) throw std::invalid_argument("badly formatted move: " + s);

        auto begin = Position(s[0] - 'a', s[1] - '1');
        auto end   = Position(s[3] - 'a', s[4] - '1');
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

struct Board : std::array<std::array<Square, 8>, 8> {
    static Board from_fen(const std::string&);
    Board()
        : std::array<std::array<Square, 8>, 8>{}
    {}

    Square& at(const Position& p) { return (*this)[p.col()][p.row()]; }
    const Square& at(const Position& p) const { return (*this)[p.col()][p.row()]; }
};
std::string to_fen(const Board&);

struct State {
    static State from_fen(const std::string&);

    Board board{};
    Player active_player = Player::white;
    Castle_rights white_castle_rights = Castle_rights::both;
    Castle_rights black_castle_rights = Castle_rights::both;
    Position en_passant_target = "a1";  // Use a1 position to mean 'no en passant square'
    unsigned halfmove_clock = 0;
    unsigned fullmove_number = 0;
};
std::string to_fen(const State&);

}  // namespace Chess

