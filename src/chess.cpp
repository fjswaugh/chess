#include "chess/chess.h"

#include "m120.h"

#include <algorithm>

namespace Chess {

State apply(const Move& m, const State& s)
{
    State120 state = s;
    state.apply(m);
    return state;
}

double evaluate(const State& s)
{
    const bool maximise = s.active_player == Player::white;
    return minimax_evaluate(4, s, maximise);
}

std::vector<Move> generate_moves(const State& s)
{
    const auto moves = generate_moves(s.board, s.active_player);
    std::vector<Move> result;
    result.reserve(moves.size());
    std::transform(moves.begin(), moves.end(), std::back_inserter(result),
                   [](auto m) -> Move { return m; });
    return result;
}

Move calculate_best_move(const State& s)
{
    return calculate_best_move(State120{s});
}

bool is_legal_move(const Move& m, const State& s)
{
    auto valid_moves = generate_moves(s.board, s.active_player);
    add_castling_moves(s.board, s.active_player, valid_moves);
    State120 state = s;
    Move120 move = m;

    if (std::find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end()) {
        return false;
    }

    state.apply(move);
    if (in_check(state.board, s.active_player)) {
        return false;
    }

    return true;
}

}  // namespace Chess

