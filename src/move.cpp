#include "chess/move.h"
#include "chess/position.h"

namespace Chess {

Move deduce_move_from_coordinates(const Position& position, Location from, Location to,
                                  Piece promotion_type)
{
    const int far_row = position.active_player == Player::white ? 7 : 0;

    if (to == position.en_passant_target && position.en_passant_target != "a1") {
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
        switch (promotion_type) {
            case Piece::rook:
                info = Move::Info::rook_promotion;
                break;
            case Piece::knight:
                info = Move::Info::knight_promotion;
                break;
            case Piece::bishop:
                info = Move::Info::bishop_promotion;
                break;
            case Piece::queen:
            default:
                info = Move::Info::queen_promotion;
                break;
        }
    }

    if (is_square_owned_by_player(position.mailbox[to], opponent_of(position.active_player))) {
        info = Move::Info(*info | *Move::Info::normal_capture);
    }

    return Move(from, to, info);
}

}  // namespace Chess

