#pragma once

#include "chess/misc.h"

#include <array>
#include <algorithm>

namespace Chess {

/**
 * Simple type to hold a standard 8x8 board representation
 */
struct Mailbox : std::array<Square, 64> {
    Mailbox() { std::fill(this->begin(), this->end(), Square::empty); }
};

}  // namespace Chess

