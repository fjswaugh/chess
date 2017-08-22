#include "chess/evaluate.h"
#include "chess/generate_moves.h"

namespace Chess {

constexpr double min_int = -10000;
constexpr double max_int = 10000;

double sum_pieces(const Position& position)
{
    double score = 0;
    for (const auto square : position.mailbox) {
        double piece_value;

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

double minimax_evaluate(const Position& position, int depth)
{
    if (depth <= 0) return sum_pieces(position);

    const bool maximise = position.active_player == Player::white;

    const auto moves = generate_moves(position);

    double best_score = (maximise ? min_int : max_int);

    for (const auto& move : moves) {
        const auto new_position = apply(move, position);
        const auto score = minimax_evaluate(new_position, depth - 1);

        if ((maximise && score > best_score) || (!maximise && score < best_score)) {
            best_score = score;
        }
    }

    return best_score;
}

double evaluate(const Position& position)
{
    return minimax_evaluate(position, 4);
}

Move calculate_best_move(const Position& position)
{
    constexpr int depth = 5;
    const bool maximise = position.active_player == Player::white;

    const auto moves = generate_moves(position);
    if (moves.empty()) throw std::runtime_error("Cannot find a legit move");

    double best_score = (maximise ? min_int : max_int);
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

