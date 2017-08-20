#include "chess/move.h"
#include "chess/position.h"

namespace Chess {

Move deduce_move_from_coordinates(Location from, Location to, const Position& position)
{
    const int far_row = position.active_player == Player::white ? 7 : 0;

    if (to == position.en_passant_target) {
        return Move(from, to, Move::Info::en_passant_capture);
    }

    if (to_piece(position.mailbox[from]) == Piece::pawn && std::abs(to.row() - from.row()) == 2) {
        return Move(from, to, Move::Info::double_pawn_push);
    }

    if (to_piece(position.mailbox[from]) == Piece::king && to.col() - from.col() == 2) {
        return Move(from, to, Move::Info::kingside_castle);
    }

    if (to_piece(position.mailbox[from]) == Piece::king && from.col() - to.col() == 2) {
        return Move(from, to, Move::Info::queenside_castle);
    }

    auto info = Move::Info(0);

    if (to.row() == far_row && to_piece(position.mailbox[from]) == Piece::pawn) {
        info = Move::Info(*info | *Move::Info::normal_promotion);
    }

    if (is_square_owned_by_player(position.mailbox[to], opponent_of(position.active_player))) {
        info = Move::Info(*info | *Move::Info::normal_capture);
    }

    return Move(from, to, info);
}

}  // namespace Chess

