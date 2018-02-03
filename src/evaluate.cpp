#include "chess/evaluate.h"
#include "chess/generate_moves.h"
#include "chess/transposition_table.h"

#include <algorithm>
#include <utility>
#include <memory>
#include <tuple>

namespace Chess {

namespace {

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

constexpr i16 big = 10000;

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

i16 invert_if_black(i16 score, Player p) {
    return score * (i16(p) * (-2) + 1);
}

/**
 * Generates moves and then orders them according to whatever heuristics we
 * wish to use. Right now, we simply swap the front move with the one we deem
 * most likely to be best.
 */
auto generate_ordered_moves(const Position& position, const u64 key,
                            const Transposition_node& node)
{
    auto moves = generate_moves(position);
    if (node && node.key == key) {
        std::swap(moves[0], moves[node.best_move_position]);
    }
    return moves;
}

Recommendation search(const Io& io, const Position& position, u8 depth, i16 alpha, i16 beta,
                      Transposition_table& tt)
{
    if (depth == 0) {
        return {{}, invert_if_black(static_evaluate(position), position.active_player)};
    }

    const auto key = zobrist_hash(position);
    const auto& node = tt.at(key);

    // If we've already encountered this exact situation before, we can just return
    if (node && node.key == key && node.depth >= depth && node.type == Node_type::pv) {
        return {node.best_move, node.score};
    }

    const auto moves = generate_ordered_moves(position, key, node);

    Move best_move = moves.empty() ? Move{} : moves.front();
    u8 best_move_position = 0;

    u8 i = 0;
    for (const auto& move : moves) {
        if (io.stopped()) return {best_move, alpha};

        const auto new_position = apply(move, position);
        const auto score = -search(io, new_position, depth - 1, -beta, -alpha, tt).score;

        if (score >= beta) {
            alpha = beta;
            best_move = move;
            best_move_position = i;
            break;
        }
        if (score > alpha) {
            alpha = score;
            best_move = move;
            best_move_position = i;
        }

        ++i;
    }

    const Node_type type = alpha == beta ? Node_type::fail_high : Node_type::pv;
    tt.at(key) = Transposition_node{key, best_move, best_move_position, alpha, depth, type};

    return {best_move, alpha};
}

}  // namespace

void recommend_move(const Io& io, const Position& position, Transposition_table& tt)
{
    const u8 max_depth = 7;

    Move best_move;
    i16 best_score{};

    for (u8 depth = 1; depth <= max_depth; ++depth) {
        const i16 alpha = -big;
        const i16 beta  = +big;
        const auto [move, score] = search(io, position, depth, alpha, beta, tt);

        if (io.stopped()) break;

        best_move = move;
        best_score = score;
    }

    io.report_score(best_score);
    io.report_best_move(best_move);
}

}  // namespace Chess

