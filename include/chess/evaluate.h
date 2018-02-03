#pragma once

#include "chess/position.h"
#include "chess/transposition_table.h"

#include <atomic>
#include <functional>
#include <utility>

namespace Chess {

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

struct Recommendation {
    Move move;
    i16 score;
};

/**
 * Main function for selecting a move
 */
void recommend_move(const Io&, const Position&, Transposition_table&);

inline void recommend_move(const Io& io, const Position& p)
{
    Transposition_table tt;
    recommend_move(io, p, tt);
}

inline Recommendation recommend_move(const Position& position, Transposition_table& tt)
{
    Recommendation result;

    Io io;
    io.report_best_move = [&result](Move m) { result.move = m; };
    io.report_score     = [&result](i16 s) { result.score = s; };

    recommend_move(io, position, tt);

    return result;
}

inline Recommendation recommend_move(const Position& position)
{
    Transposition_table tt;
    return recommend_move(position, tt);
}

// Need evaluate functions

}  // namespace Chess

