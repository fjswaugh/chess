#include "chess/evaluate.h"
#include "chess/generate_moves.h"

namespace Chess {

int sum_pieces(const Position& position)
{
    int score = 0;
    for (const auto square : position.mailbox) {
        int piece_value;

        switch (to_piece(square)) {
            case Piece::rook:
                piece_value = 5; break;
            case Piece::queen:
                piece_value = 9; break;
            case Piece::bishop:
                piece_value = 3; break;
            case Piece::knight:
                piece_value = 3; break;
            case Piece::pawn:
                piece_value = 1; break;
            default:
                piece_value = 0; break;
        };

        if (is_black(square)) piece_value = -piece_value;

        score += piece_value;
    }

    return score;
}

int minimax_evaluate(const Position& position, int depth)
{
    if (depth <= 0) return sum_pieces(position);

    const bool maximise = position.active_player == Player::white;

    const auto moves = generate_moves(position);

    int best_score = (maximise ? std::numeric_limits<int>::min() :
                                    std::numeric_limits<int>::max());

    for (const auto& move : moves) {
        const auto new_position = apply(move, position);
        const auto score = minimax_evaluate(new_position, depth - 1);

        if ((maximise && score > best_score) || (!maximise && score < best_score)) {
            best_score = score;
        }
    }

    return best_score;
}

Move calculate_best_move(const Position& position)
{
    constexpr int depth = 4;
    const bool maximise = position.active_player == Player::white;

    const auto moves = generate_moves(position);
    if (moves.empty()) throw std::runtime_error("Cannot find a legit move");

    int best_score = (maximise ? std::numeric_limits<int>::min()
                               : std::numeric_limits<int>::max());
    auto best_move = moves.front();

    for (const auto& move : moves) {
        const auto new_position = apply(move, position);
        const auto score = minimax_evaluate(new_position, depth - 1);

        if ((maximise && score > best_score) || (!maximise && score < best_score)) {
            best_score = score;
            best_move  = move;
        }
    }

    return best_move;
}

}  // namespace Chess

