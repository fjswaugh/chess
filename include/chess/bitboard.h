#pragma once

#include "chess/misc.h"
#include "chess/location.h"
#include "chess/move.h"

#include <cstdint>
#include <cassert>
#include <bitset>

namespace Chess {

/**
 *
 * Consider a chess board indexed as followed
 *   +-----------------------+
 * 8 |56|57|58|59|60|61|62|63|
 *   +--|--|--|--|--|--|--|--+
 * 7 |48|49|50|51|52|53|54|55|
 *   +--|--|--|--|--|--|--|--+
 * 6 |40|41|42|43|44|45|46|47|
 *   +--|--|--|--|--|--|--|--+
 * 5 |32|33|34|35|36|37|38|29|
 *   +--|--|--|--|--|--|--|--+
 * 4 |24|25|26|27|28|29|30|31|
 *   +--|--|--|--|--|--|--|--+
 * 3 |16|17|18|19|20|21|22|23|
 *   +--|--|--|--|--|--|--|--+
 * 2 | 8| 9|10|11|12|13|14|15|
 *   +--|--|--|--|--|--|--|--+
 * 1 | 0| 1| 2| 3| 4| 5| 6| 7|
 *   +-----------------------+
 *     A  B  C  D  E  F  G  H
 *
 * Those indices represent the positions of those squares in a 64-bit bitboard,
 * with the 0 index being the least significant bit, and 63 representing the
 * most significant bit
 *
 * That is, an otherwise empty board with pieces at A3 and B2 would be
 * represented by the following 64-bit unsigned integer:
 *
 * 00000000 00000000 00000000 00000000 00000000 00000000 00000010 00000100 
 *
 */

using Bitboard = u64;

inline constexpr Bitboard mask_of(Location p) {
    return Bitboard(1) << p;
}

inline constexpr int count(Bitboard b) {
    return __builtin_popcountl(b);
}

}  // namespace Chess

