#pragma once

#include <string>
#include <cassert>
#include <array>
#include <vector>
#include <iostream>

#include "move.h"
#include "position.h"
#include "player.h"
#include "square.h"

struct Mailbox {
    static constexpr int size = 120;

    static Mailbox from_fen(const std::string&);

    const Square& square(int i) const { return square_array[i]; }
    const Square& square(Position p) const { return square_array[p.value()]; }

    void apply(const Move&);

    Mailbox()
    {
        constexpr auto e = Square::empty;
        constexpr auto o = Square::off_board;

        square_array = {{
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
        }};
    }
private:
    // This is private for now, but might be more sensible (read: flexible) as public, we'll see.
    // For now we'll keep it private and maintain the class invariant that all the edges are
    // off_board (restricted by the square functions taking a position that must be on board).
    std::array<Square, size> square_array;
};

std::vector<Move> generate_moves(const Mailbox&, Player);
std::string to_fen(const Mailbox&);
bool in_check(const Mailbox&, Player);

