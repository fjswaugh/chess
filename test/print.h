#pragma once

#include "chess/chess.h"
#include "rang.hpp"

inline void pretty_print(const Chess::Mailbox& b)
{
    using namespace Chess;
    for (auto row = 8; row-- > 0;) {
        for (auto col = 0; col < 8; ++col) {
            if ((row + col) % 2 == 0) {
                std::cout << rang::bg::green;
            } else {
                std::cout << rang::bg::gray;
            }
            std::cout << rang::fg::black << to_unicode(b[Location(col, row)]);
        }
        std::cout << '\n';
    }
    std::cout << rang::style::reset;
}

inline void pretty_print(const Chess::Position& p)
{
    using namespace Chess;
    std::cout << (p.active_player == Player::white ? "White" : "Black") << " to move\n";
    pretty_print(p.mailbox);
}

inline void print(Chess::Bitboard b)
{
    std::bitset<64> bs = b;
    int i = 64 - 8;
    while (i >= 0) {
        std::printf("%i", bs.test(i));
        ++i;
        if (i % 8 == 0) {
            std::printf("\n");
            i -= 16;
        }
    }
    std::printf("\n");
}

