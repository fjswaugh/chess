#pragma once

#include "chess/location.h"
#include "chess/bitboard.h"
#include "chess/move.h"
#include "chess/mailbox.h"
#include "chess/misc.h"

#include <string>
#include <istream>

namespace Chess {

/**
 * The main type representing a chess position
 * It contains both bitboard and 'mailbox' representations of the chess board,
 * with the redundancy for convenience.
 */
struct Position {
    static Position from_fen(const std::string& fen_str);

    Mailbox mailbox;
    Bitboard bitboard_by_square[12] = {};
    Bitboard bitboard_by_player[2] = {};

    Castle_rights castling[2] = {Castle_rights::both, Castle_rights::both};
    //Location king_location_for_player[2] = {"e1", "e8"};
    Location en_passant_target = "a1";  // Use a1 for no en passant square
    Player active_player = Player::white;
    int halfmove_clock = 0;
    int fullmove_number = 1;
};

/**
 * Produces the Zobrist hash of a position, see
 * https://en.wikipedia.org/wiki/Zobrist_hashing
 */
u64 zobrist_hash(const Position&);

std::string to_fen(const Position&);
std::istream& operator>>(std::istream&, Position&);

/**
 * Moves can be applied to positions, with the result being an entirely new
 * position. It may be more efficient to apply the move in place, but such a
 * strategy would necessitate a complex 'unapply' function.
 */
Position apply(Move, Position);

}  // namespace Chess

