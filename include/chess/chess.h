#pragma once

#include "chess/game.h"
#include "chess/misc.h"
#include "chess/square.h"
#include "chess/state.h"

#include <vector>

namespace Chess {

double evaluate(const State&);

std::vector<Move> generate_moves(const State&);

Move calculate_best_move(const State&);

// Possibly also something like this
//std::vector<Move> calculate_best_moves(const State&);

bool is_legal_move(const Move&, const State&);

State apply(const Move&, const State&);

}  // namespace Chess
