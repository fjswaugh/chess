#pragma once

#include <string>
#include <array>
#include <vector>

#include "chess/square.h"
#include "chess/misc.h"
#include "chess/state.h"

namespace Chess {

struct Position120 {
    Position120(Chess::Position p)
        : data_(p.col() + 21 + p.row() * 10)
    {}
    operator Chess::Position() {
        return Chess::Position(this->col(), this->row());
    }
    Position120(int i)
        :data_{i}
    {}
    operator int() const {
        return data_;
    }
    int col() const {
        return data_ % 10 - 1;
    }
    int row() const {
        return data_ / 10 - 2;
    }
private:
    int data_;
};

struct Move120 {
    Move120(Position120 begin, Position120 end) 
        : begin{begin}, end{end}
    {}
    Move120(Chess::Move m)
        : begin{Position120{m.begin}}, end{Position120{m.end}}
    {}
    operator Chess::Move() {
        return Chess::Move{static_cast<Chess::Position>(begin), static_cast<Chess::Position>(end)};
    }
    Position120 begin;
    Position120 end;
};

inline bool operator==(const Move120& a, const Move120& b) {
    return a.begin == b.begin && a.end == b.end;
}

struct Board120 : std::array<Square, 120> {
    void apply(const Move120&);

    Board120()
        : std::array<Square, 120>{{
            o, o, o, o, o, o, o, o, o, o,
            o, o, o, o, o, o, o, o, o, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, e, e, e, e, e, e, e, e, o,
            o, o, o, o, o, o, o, o, o, o,
            o, o, o, o, o, o, o, o, o, o
        }}
    {}

    Board120(const Chess::Board& b)
        : Board120()
    {
        for (int col = 0; col < 8; ++col) {
            for (int row = 0; row < 8; ++row) {
                (*this)[col + 21 + row * 10] = b[col][row];
            }
        }
    }

    operator Chess::Board() const {
        Chess::Board b;
        for (int col = 0; col < 8; ++col) {
            for (int row = 0; row < 8; ++row) {
                 b[col][row] = (*this)[col + 21 + row * 10];
            }
        }
        return b;
    }
private:
    static constexpr Square e = Square::empty;
    static constexpr Square o = Square::off_board;
};

struct State120 {
    State120(const Chess::State& state)
        : board{state.board}, active_player{state.active_player}
    {}

    operator Chess::State() const {
        Chess::State s;
        s.board = board;
        s.active_player = active_player;
        // Do other fields here
        return s;
    }

    void apply(const Move120&);
    Board120 board;
    Player active_player;
};

std::vector<Move120> generate_moves(const Board120&, Player);
void add_castling_moves(const Board120&, Player, std::vector<Move120>&);
bool in_check(const Board120&, Player);
Move120 calculate_best_move(const State120&);
int minimax_evaluate(int depth, const State120&, bool maximise);

}  // namespace Chess

