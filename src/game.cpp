#include "chess/chess.h"

namespace Chess {

Game::Game() = default;

Game::Game(const Position& initial_position)
    : history_{initial_position}
{}

Game::Game(Position&& initial_position)
    : history_{std::move(initial_position)}
{}

void Game::add_state(const Position& p)
{
    history_.push_back(p);
}

void Game::add_state(Position&& p)
{
    history_.push_back(std::move(p));
}

void Game::undo()
{
    if (history_.size() > 1) history_.pop_back();
    if (history_.size() > 1) history_.pop_back();
}

}
