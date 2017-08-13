#pragma once

#include <vector>

#include "chess/state.h"

namespace Chess {

class Game {
public:
    Game(State initial_state = {})
        : history_{initial_state}
    {}

    State& state() { return history_.back(); }
    const State& state() const { return history_.back(); }

    void add_state(State s) {
        history_.push_back(s);
    }

    void undo() {
        if (history_.size() > 1) history_.pop_back();
        if (history_.size() > 1) history_.pop_back();
    }
private:
    std::vector<State> history_;
};

}  // namespace Chess

