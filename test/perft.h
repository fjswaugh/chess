#pragma once

#include "chess/generate_moves.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <vector>
#include <array>

inline long long count_moves_single(const Chess::Position& position, int ply)
{
    if (ply < 1) return 1;

    long long counter = 0;

    const auto moves = Chess::generate_moves(position);

    if (ply == 1) return moves.size();

    for (auto move : moves) {
        const auto new_position = apply(move, position);
        counter += count_moves_single(new_position, ply - 1);
    }

    return counter;
}

inline long long count_moves(const Chess::Position& position, int ply)
{
    if (ply < 1) return 1;

    const auto moves = Chess::generate_moves(position);

    if (ply == 1) return moves.size();

    constexpr int num_threads = 4;

    const auto f = [&position, ply](auto begin, auto end, long long& counter) {
        for (auto it = begin; it != end; ++it) {
            const auto& move = *it;
            const auto new_position = apply(move, position);
            counter += count_moves_single(new_position, ply - 1);
        }
        return counter;
    };

    auto block_size = moves.size() / num_threads;
    std::array<long long, num_threads> counters{};
    std::array<std::thread, num_threads - 1> threads;

    auto block_begin = moves.begin();
    for (int i = 0; i < num_threads - 1; ++i) {
        auto block_end = block_begin;
        std::advance(block_end, block_size);
        threads[i] = std::thread(f, block_begin, block_end, std::ref(counters[i]));
        block_begin = block_end;
    }

    f(block_begin, moves.end(), counters.back());

    for (auto& thread : threads) {
        thread.join();   
    }

    return std::accumulate(counters.begin(), counters.end(), 0LL);
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

