#pragma once

#include "chess/misc.h"
#include "chess/move.h"
#include "chess/move_list.h"
#include "chess/position.h"

namespace Chess {

Move_list generate_moves(const Position&);

bool is_legal_move(Move, const Position&);

}

