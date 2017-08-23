#pragma once

#include "chess/position.h"

#include <utility>

namespace Chess {

std::pair<Move, int> recommend_move(const Position&);
int evaluate(const Position&);

}  // namespace Chess

