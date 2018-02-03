#pragma once

#include "chess/position.h"
#include "chess/transposition_table.h"

#include <atomic>
#include <functional>
#include <utility>

namespace Chess {

/**
 * Object for communicating with an evaluation function
 */
struct Io {
    Io() = default;

    template <typename F1, typename F2>
    Io(F1&& f1, F2&& f2)
        : report_best_move{std::forward<F1>(f1)}, report_score{std::forward<F2>(f2)}
    {}

    void stop() { stop_ = true; }
    void go()   { stop_ = false; }

    bool stopped() const { return stop_; }

    std::function<void(Move)> report_best_move = [](Move){};
    std::function<void(i16)>  report_score     = [](i16){};
private:
    std::atomic<bool> stop_ = false;
};

/**
 * A move recommendation type, containing just the recommended move and a score
 * associated with the position. The score places a value of 100 on each pawn's
 * worth of advantage held by a player.
 */
struct Recommendation {
    Move move;
    i16 score;
};

/**
 * Main function for selecting a move, the output is given to the Io object
 * passed in. The transposition table passed in will be used and modified.
 */
void recommend_move(const Io&, const Position&, Transposition_table&);

/**
 * Convenience function for recommending a move without having to pass in an Io
 * object
 */
inline Recommendation recommend_move(const Position& position, Transposition_table& tt)
{
    Recommendation result;

    Io io;
    io.report_best_move = [&result](Move m) { result.move = m; };
    io.report_score     = [&result](i16 s) { result.score = s; };

    recommend_move(io, position, tt);

    return result;
}

/**
 * Convenience function to recommend a move without needing to pass in a
 * transposition table
 */
inline Recommendation recommend_move(const Position& position)
{
    // Create a small transposition table
    Transposition_table tt(8);
    return recommend_move(position, tt);
}

/**
 * Convenience function to evaluate a position
 */
inline i16 evaluate(const Position& position)
{
    return recommend_move(position).score;
}

}  // namespace Chess

