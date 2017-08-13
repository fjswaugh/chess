#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <random>
#include <array>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "chess/misc.h"
#include "chess/square.h"
#include "m120.h"

namespace Chess {

void add_castling_moves(const Board120& b, Player player, std::vector<Move120>& moves)
{
    const Position120 king_initial_position = Position(player == Player::white ? "e1" : "e8");
    const Square king_square = b[king_initial_position];

    if ((player == Player::white && king_square != Square::white_king) ||
        (player == Player::black && king_square != Square::black_king))
    {
        return;
    }

    if (in_check(b, player)) return;

    // Check Queenside
    [&] {
        if (!is_empty(b[king_initial_position - 1])) return;
        if (!is_empty(b[king_initial_position - 2])) return;
        if (!is_empty(b[king_initial_position - 3])) return;

        const auto rook_square = b[king_initial_position - 4];

        if ((player == Player::white && rook_square != Square::white_rook) ||
            (player == Player::black && rook_square != Square::black_rook))
        {
            return;
        }

        auto b_copy = b;
        b_copy.apply(Move120{king_initial_position, king_initial_position - 1});
        if (in_check(b_copy, player)) return;

        moves.emplace_back(king_initial_position, king_initial_position - 2);
    }();

    // Check Kingside
    [&] {
        if (!is_empty(b[king_initial_position + 1])) return;
        if (!is_empty(b[king_initial_position + 2])) return;

        const auto rook_square = b[king_initial_position + 3];

        if ((player == Player::white && rook_square != Square::white_rook) ||
            (player == Player::black && rook_square != Square::black_rook))
        {
            return;
        }

        auto b_copy = b;
        b_copy.apply(Move120{king_initial_position, king_initial_position + 1});
        if (in_check(b_copy, player)) return;

        moves.emplace_back(king_initial_position, king_initial_position + 2);
    }();
}

std::vector<Move120> generate_moves(const Board120& b, Player player)
{
    const auto opponent = opponent_of(player);
    std::vector<Move120> moves;

    const auto add_linear_moves = [&](int p, int offset) {
        int destination = p + offset;
        while (is_empty(b[destination]) || is_square_owned_by_player(b[destination], opponent)) {
            moves.emplace_back(p, destination);
            if (is_square_owned_by_player(b[destination], opponent)) break;
            destination += offset;
        }
    };

    const auto add_direct_move = [&](int p, int offset) {
        int destination = p + offset;
        if (is_empty(b[destination]) || is_square_owned_by_player(b[destination], opponent)) {
            moves.emplace_back(p, destination);
        }
    };

    const auto add_pawn_moves = [&](int p) {
        // Forward direction for player
        const int dir = (player == Player::white ? 1 : -1);

        if (is_empty(b[p + dir * 10])) {
            moves.emplace_back(p, p + dir * 10);

            // If the square in front is empty then consider moving two squares (but only if we're
            // on the right row)
            if (((player == Player::white && p < 39 && p > 30) ||
                 (player == Player::black && p < 89 && p > 80)) &&
                is_empty(b[p + dir * 20]))
            {
                moves.emplace_back(p, p + dir * 20);
            }
        }

        const auto add_pawn_take_move = [&](int offset) {
            const int destination = p + offset;
            if (is_square_owned_by_player(b[destination], opponent)) {
                moves.emplace_back(p, destination);
            }
        };
        add_pawn_take_move(dir * 9);
        add_pawn_take_move(dir * 11);
    };

    for (int i = 0; i < 120; ++i) {
        const auto square = b[i];

        if (is_square_owned_by_player(square, player)) {
            const auto piece = to_piece(square);

            switch (piece) {
            case Piece::empty:
                break;
            case Piece::pawn:
                add_pawn_moves(i);
                break;
            case Piece::rook:
                add_linear_moves(i, 10);
                add_linear_moves(i, 1);
                add_linear_moves(i, -1);
                add_linear_moves(i, -10);
                break;
            case Piece::knight:
                add_direct_move(i, 21);
                add_direct_move(i, 12);
                add_direct_move(i, -8);
                add_direct_move(i, -19);
                add_direct_move(i, -21);
                add_direct_move(i, -12);
                add_direct_move(i, 8);
                add_direct_move(i, 19);
                break;
            case Piece::bishop:
                add_linear_moves(i, 11);
                add_linear_moves(i, -9);
                add_linear_moves(i, -11);
                add_linear_moves(i, 9);
                break;
            case Piece::queen:
                add_linear_moves(i, 10);
                add_linear_moves(i, 1);
                add_linear_moves(i, -1);
                add_linear_moves(i, -10);
                add_linear_moves(i, 11);
                add_linear_moves(i, -9);
                add_linear_moves(i, -11);
                add_linear_moves(i, 9);
                break;
            case Piece::king:
                add_direct_move(i, 1);
                add_direct_move(i, 9);
                add_direct_move(i, 10);
                add_direct_move(i, 11);
                add_direct_move(i, -1);
                add_direct_move(i, -9);
                add_direct_move(i, -10);
                add_direct_move(i, -11);
                break;
            }
        }
    }

    return moves;
}

void Board120::apply(const Move120& m)
{
    // If this is a castling move, then move the rook into place
    if (to_piece((*this)[m.begin]) == Piece::king && std::abs(m.end - m.begin) == 2) {
        const bool kingside = m.end > m.begin;
        const bool white = is_square_owned_by_player((*this)[m.begin], Player::white);

        Position120 rook_starting_position  = Position((kingside ? 7 : 0), white ? 0 : 7);
        Position120 rook_finishing_position = Position((kingside ? 5 : 3), white ? 0 : 7);

        this->apply(Move120{rook_starting_position, rook_finishing_position});
    }

    auto& square_to_be_moved = (*this)[m.begin];
    square_to_be_moved = remove_castling_rights(square_to_be_moved);

    (*this)[m.end]= (*this)[m.begin];
    (*this)[m.begin] = Square::empty;

    // Apply pawn promotion to queen
    if (to_piece((*this)[m.end]) == Piece::pawn && (m.end.row() == 0 || m.end.row() == 7)) {
        (*this)[m.end] = promote_to((*this)[m.end], Piece::queen);
    }
}

void State120::apply(const Move120& move) {
    this->board.apply(move);
    this->active_player = opponent_of(this->active_player);
}

bool in_check(const Board120& b, Player p)
{
    const Position120 king_position = [&] {
        for (int i = 0; i < 120; ++i) {
            if (to_piece(b[i]) == Piece::king && is_square_owned_by_player(b[i], p)) {
                return i;
            }
        }
        // No king???
        assert(false && "No king found in mailbox");
    }();

    const auto moves = generate_moves(b, opponent_of(p));

    for (const auto& move : moves) {
        if (move.end == king_position) return true;
    }
    return false;
}

int sum_pieces(const Board120& board)
{
    int score = 0;
    for (const auto square : board) {
        int piece_value;

        switch (to_piece(square)) {
            case Piece::rook:
                piece_value = 5; break;
            case Piece::queen:
                piece_value = 10; break;
            case Piece::bishop:
                piece_value = 3; break;
            case Piece::king:
                piece_value = 1000000; break;
            case Piece::knight:
                piece_value = 3; break;
            case Piece::pawn:
                piece_value = 1; break;
            default:
                piece_value = 0; break;
        };

        if (is_black(square)) piece_value *= -1;

        score += piece_value;
    }

    return score;
}

int minimax_evaluate(int depth, const State120& state, bool maximise)
{
    if (depth <= 0) return sum_pieces(state.board);

    auto moves = generate_moves(state.board, state.active_player);
    add_castling_moves(state.board, state.active_player, moves);

    int best_score = (maximise ? std::numeric_limits<int>::min() :
                                 std::numeric_limits<int>::max());

    for (const auto& move : moves) {
        auto state_copy = state;
        state_copy.apply(move);
        const auto score = minimax_evaluate(depth - 1, state_copy, !maximise);

        if ((maximise && score > best_score) || (!maximise && score < best_score)) {
            best_score = score;
        }
    }

    return best_score;
}

Move120 calculate_best_move(const State120& state)
{
    constexpr int depth = 4;
    const bool maximise = state.active_player == Player::white;

    auto moves = generate_moves(state.board, state.active_player);
    add_castling_moves(state.board, state.active_player, moves);
    if (moves.empty()) throw std::runtime_error("Cannot find a legit move");

    int best_score = (maximise ? std::numeric_limits<int>::min()
                               : std::numeric_limits<int>::max());
    auto best_move = moves.front();

    for (const auto& move : moves) {
        auto state_copy = state;
        state_copy.apply(move);
        const auto score = minimax_evaluate(depth - 1, state_copy, !maximise);

        if ((maximise && score > best_score) || (!maximise && score < best_score)) {
            best_score = score;
            best_move  = move;
        }
    }

    return best_move;
}

}  // namespace Chess

