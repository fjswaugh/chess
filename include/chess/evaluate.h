#pragma once

#include "chess/position.h"

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

std::pair<Move, int> recommend_move(Io&, const Position&);
int evaluate(const Position&);

}  // namespace Chess

