#pragma once

#include "chess/position.h"
#include "chess/transposition_table.h"

#include <utility>
#include <atomic>

namespace Chess {

struct Io {
    Io() : stop_{0} {}

    void stop() { stop_ = 1; }
    void go() { stop_ = 0; }

    bool stopped() const { return stop_; }
private:
    std::atomic<int> stop_;
};

std::pair<Move, i16> recommend_move(Io&, const Position&, Transposition_table);
i16 evaluate(const Position&);

}  // namespace Chess

