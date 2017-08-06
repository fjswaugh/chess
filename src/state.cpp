#include "chess/state.h"
#include "chess/move.h"
#include "chess/mailbox.h"
#include <random>
#include <limits>

int sum_pieces(const Mailbox& m)
{
    int score = 0;
    for (int i = 0; i < m.size; ++i) {
        auto piece = to_piece(m.square(i));

        int piece_value;
        switch (piece) {
            case Piece::rook:
                piece_value = 5; break;
            case Piece::queen:
                piece_value = 10; break;
            case Piece::bishop:
                piece_value = 3; break;
            case Piece::king:
                piece_value = 100000000; break;
            case Piece::knight:
                piece_value = 3; break;
            case Piece::pawn:
                piece_value = 1; break;
            default:
                piece_value = 0; break;
        };

        if (is_black(m.square(i))) {
            piece_value *= -1;
        }

        score += piece_value;
    }

    return score;
}

int minimax_evaluate(int depth, const State& s, bool maximise)
{
    if (depth <= 0) return sum_pieces(s.mailbox);

    auto moves = generate_moves(s.mailbox, s.active_player);

    int best_score = (maximise ? std::numeric_limits<int>::min() :
                                 std::numeric_limits<int>::max());

    for (const auto move : moves) {
        auto s_copy = s;
        s_copy.apply(move);
        auto score = minimax_evaluate(depth - 1, s_copy, !maximise);

        if ((maximise  && score > best_score) ||
            (!maximise && score < best_score))
        {
            best_score = score;
        }
    }
    return best_score;
}

Move calculate_best_move(const State& s)
{
    constexpr int depth = 4;
    const bool maximise = s.active_player == Player::white;

    auto moves = generate_moves(s.mailbox, s.active_player);
    if (moves.empty()) throw std::runtime_error("Cannot find a legit move");

    int best_score = (maximise ? std::numeric_limits<int>::min()
                               : std::numeric_limits<int>::max());
    auto best_move = moves.front();

    for (auto&& move : moves) {
        auto s_copy = s;
        s_copy.apply(move);
        auto score = minimax_evaluate(depth - 1, s_copy, !maximise);

        if ((maximise && score > best_score) ||
            (!maximise && score < best_score))
        {
            best_score = score;
            best_move  = move;
        }
    }

    return best_move;
}

