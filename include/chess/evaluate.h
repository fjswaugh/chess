#pragma once

#include "chess/position.h"

namespace Chess {

Move calculate_best_move(const Position&);
double evaluate(const Position&);

}  // namespace Chess

