#pragma once

#include "chess/position.h"

namespace Chess {

Move calculate_best_move(const Position&);
int minimax_evaluate(const Position&, int depth);

}  // namespace Chess

