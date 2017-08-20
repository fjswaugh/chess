#pragma once

#include "chess/misc.h"

#include <array>
#include <algorithm>

namespace Chess {

struct Mailbox : std::array<Square, 64> {
    Mailbox() { std::fill(this->begin(), this->end(), Square::empty); }
};

}  // namespace Chess

