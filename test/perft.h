#pragma once

#include "chess/generate_moves.h"
#include <iostream>
#include <algorithm>

inline long long count_moves(const Chess::Position& position, int ply)
{
    if (ply < 1) return 1;

    long long counter = 0;

    const auto moves = Chess::generate_moves(position);

    if (ply == 1) return moves.size();

    for (auto move : moves) {
        const auto new_position = apply(move, position);
        counter += count_moves(new_position, ply - 1);
    }

    return counter;
}

inline void divide(const Chess::Position& position, int ply)
{
    if (ply < 1) {
        std::cout << "1\n";
        return;
    }

    const auto moves = Chess::generate_moves(position);
    long long total = 0;

    std::cout << "Move  | Nodes\n"
              << "------|------\n";
    
    for (auto move : moves) {
        auto count = count_moves(apply(move, position), ply - 1);
        total += count;
        std::cout << to_coordinate_string(move) << " | " << count << '\n';
    }

    std::cout << "\nTotal nodoes: " << total << '\n';
}

