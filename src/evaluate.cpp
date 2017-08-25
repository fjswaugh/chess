#include "chess/evaluate.h"
#include "chess/generate_moves.h"
#include "chess/transposition_table.h"

#include <algorithm>
#include <utility>
#include <memory>
#include <tuple>

namespace Chess {

constexpr i16 piece_evaluation_tables[13][64] = {
    {  // White rooks
          0,   0,   0,   5,   5,   0,   0,   0,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
          5,  10,  10,  10,  10,  10,  10,   5,
          0,   0,   0,   0,   0,   0,   0,   0,
    }, {  // Black rooks
          0,   0,   0,   0,   0,   0,   0,   0,
          5,  10,  10,  10,  10,  10,  10,   5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
          0,   0,   0,   5,   5,   0,   0,   0,
    }, {  // White knights
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,   0,   5,   5,   0, -20, -40,
        -30,   5,  10,  15,  15,  10,   5, -30,
        -30,   0,  15,  20,  20,  15,   0, -30,
        -30,   5,  15,  20,  20,  15,   5, -30,
        -30,   0,  10,  15,  15,  10,   0, -30,
        -40, -20,   0,   0,   0,   0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
    }, {  // Black knights
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,   0,   0,   0,   0, -20, -40,
        -30,   0,  10,  15,  15,  10,   0, -30,
        -30,   5,  15,  20,  20,  15,   5, -30,
        -30,   0,  15,  20,  20,  15,   0, -30,
        -30,   5,  10,  15,  15,  10,   5, -30,
        -40, -20,   0,   5,   5,   0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
    }, {  // White bishops
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
    }, {  // Black bishops
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
    }, {  // White queens
        -20, -10, -10,  -5,  -5, -10, -10, -20,
        -10,   0,   5,   0,   0,   0,   0, -10,
        -10,   5,   5,   5,   5,   5,   0, -10,
          0,   0,   5,   5,   5,   5,   0,  -5,
         -5,   0,   5,   5,   5,   5,   0,  -5,
        -10,   0,   5,   5,   5,   5,   0, -10,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -20, -10, -10,  -5,  -5, -10, -10, -20,
    }, {  // Black queens
        -20, -10, -10,  -5,  -5, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,   5,   5,   5,   0, -10,
         -5,   0,   5,   5,   5,   5,   0,  -5,
          0,   0,   5,   5,   5,   5,   0,  -5,
        -10,   5,   5,   5,   5,   5,   0, -10,
        -10,   0,   5,   0,   0,   0,   0, -10,
        -20, -10, -10,  -5,  -5, -10, -10, -20,
    }, {  // White king
         20,  30,  10,   0,   0,  10,  30,  20,
         20,  20,   0,   0,   0,   0,  20,  20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
    }, {  // Black king
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
         20,  20,   0,   0,   0,   0,  20,  20,
         20,  30,  10,   0,   0,  10,  30,  20,
    }, {  // White pawns
          0,   0,   0,   0,   0,   0,   0,   0,
          5,  10,  10, -20, -20,  10,  10,   5,
          5,  -5, -10,   0,   0, -10,  -5,   5,
          0,   0,   0,  20,  20,   0,   0,   0,
          5,   5,  10,  25,  25,  10,   5,   5,
         10,  10,  20,  30,  30,  20,  10,  10,
         50,  50,  50,  50,  50,  50,  50,  50,
          0,   0,   0,   0,   0,   0,   0,   0,
    }, {  // Black pawns
          0,   0,   0,   0,   0,   0,   0,   0,
         50,  50,  50,  50,  50,  50,  50,  50,
         10,  10,  20,  30,  30,  20,  10,  10,
          5,   5,  10,  25,  25,  10,   5,   5,
          0,   0,   0,  20,  20,   0,   0,   0,
          5,  -5, -10,   0,   0, -10,  -5,   5,
          5,  10,  10, -20, -20,  10,  10,   5,
          0,   0,   0,   0,   0,   0,   0,   0,
    }, {  // Empty
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,
    }
};

constexpr i16 nominal_value[13] = {
    500, 500,  // Rook
    300, 300,  // Knight
    300, 300,  // Bishop
    900, 900,  // Queen
      0,   0,  // King
    100, 100,  // Pawn
      0,       // Empty
};

i16 static_evaluate(const Position& position)
{
    i16 score = 0;

    for (int location = 0; location < 64; ++location) {
        const auto square = position.mailbox[location];
        i16 piece_value = nominal_value[*square] + piece_evaluation_tables[*square][location];
        if (is_black(square)) piece_value = -piece_value;
        score += piece_value;
    }

    return score;
}

i16 invert_if_black(Player p) {
    return i16(p) * (-2) + 1;
}

template <bool> struct Return;
template <> struct Return<false> { using type = i16; };
template <> struct Return<true>  { using type = std::pair<Move, i16>; };
template <bool root> using Return_t = typename Return<root>::type;

template <bool root>
Return_t<root> negamax(Io& io, const Position& position, u8 depth, Transposition_table tt,
                       i16 alpha = -10000, const i16 beta = 10000)
{
    if (depth <= 0) {
        if constexpr (root) {
            assert(false);
        } else {
            return static_evaluate(position) * invert_if_black(position.active_player);
        }
    }

    const auto key = zobrist_hash(position);
    Transposition_node& node = tt[index_of(key)];

    if (node && node.key == key && node.depth >= depth && node.type == Node_type::pv) {
        if constexpr (root) return {node.best_move, node.score};
        else                return node.score;
    }

    auto moves = generate_moves(position);

    if (node && node.key == key) {
        std::swap(moves[0], moves[node.best_move_position]);
    }
    
    Move best_move;

    if constexpr (root) {
        assert(!moves.empty());
        best_move = moves.front();
    }

    u8 best_move_position = 0;

    for (const auto& move : moves) {
        ++best_move_position;

        if (io.stopped()) {
            if constexpr (root) return {best_move, alpha};
            else                return alpha;
        }

        const auto new_position = apply(move, position);
        const auto score = -negamax<false>(io, new_position, depth - 1, tt, -beta, -alpha);

        if (score >= beta) {
            alpha = beta;
            break;
        }
        if (score > alpha) {
            alpha = score;
            best_move = move;
        }
    }
    --best_move_position;

    if (best_move) {
        const Node_type type = alpha == beta ? Node_type::fail_high : Node_type::pv;
        node = Transposition_node{key, best_move, best_move_position, alpha, depth, type};
    }

    if constexpr (root) return {best_move, alpha};
    else                return alpha;
}

i16 evaluate(const Position&)
{
    Io io;
    return 0;
}

std::pair<Move, i16> recommend_move(Io& io, const Position& position, Transposition_table tt)
{
    constexpr int max_depth = 7;

    auto moves = generate_moves(position);
    if (moves.empty()) throw std::runtime_error("Cannot find a legit move");

    int best_score = -10000;
    Move best_move = moves.front();

    for (u8 depth = 1; depth <= max_depth; ++depth) {
        std::tie(best_move, best_score) = negamax<true>(io, position, depth, tt);
    }

    return {best_move, best_score};
}

}  // namespace Chess

