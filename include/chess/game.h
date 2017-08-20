#pragma once

#include "chess/position.h"
#include "chess/move.h"

namespace Chess {

class Game {
public:
    Game(Position initial_position = {})
        : history_{initial_position}
    {}

    Position& position() { return history_.back(); }
    const Position& position() const { return history_.back(); }

    void add_state(Position p) {
        history_.push_back(p);
    }

    void undo() {
        if (history_.size() > 1) history_.pop_back();
        if (history_.size() > 1) history_.pop_back();
    }
private:
    std::vector<Position> history_;
};

}  // namespace Chess

