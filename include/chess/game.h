#pragma once

#include <vector>

namespace Chess {

struct Position;

/**
 * An extremely simple type that models a game of chess.
 */
class Game {
public:
    Game();
    Game(const Position& initial_position);
    Game(Position&& initial_position);

    Position& position() { return history_.back(); }
    const Position& position() const { return history_.back(); }

    void add_state(const Position&);
    void add_state(Position&&);

    void undo();
private:
    std::vector<Position> history_;
};

}  // namespace Chess

