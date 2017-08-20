#pragma once

#include "chess/position.h"

namespace Chess {

Move calculate_best_move(const Position&);
double minimax_evaluate(const Position&, int depth);

}  // namespace Chess

