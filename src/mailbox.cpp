#include <string>
#include <algorithm>
#include <random>
#include <array>
#include <vector>
#include <sstream>
#include <stdexcept>

#include <iostream>

#include "chess/mailbox.h"
#include "chess/player.h"
#include "chess/move.h"
#include "chess/square.h"

std::string to_fen(const Mailbox& m)
{
    std::string result;
    int count = 0;

    auto push_back_count = [&result, &count] {
        if (count > 0) {
            result.push_back(count + '0');
            count = 0;
        }
    };

    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            if (is_empty(m.square(Position{col, row}))) {
                ++count;
            } else {
                push_back_count();
                result.push_back(to_char(m.square(Position{col, row})));
            }
        }
        push_back_count();
        result.push_back('/');
    }
    result.pop_back();  // Pop back the last slash

    return result;
}

Mailbox Mailbox::from_fen(const std::string& s)
{
    // Assume the board is represented only in the first part of the string (ignore castling rules
    // etc. including en passant square for now)
    
    // Assume this is initialized with all empty squares
    Mailbox m;

    int row = 7;
    int col = 0;

    for (char ch : s) {
        if (ch == '/') continue;

        if (ch > '0' && ch <= '9') {
            col += (ch - '0');
        } else {
            m.square_array[static_cast<int>(Position{col, row})] = to_square(ch);
            ++col;
        }

        if (col > 7) {
            col = 0;
            --row;
        }
        if (row < 0) break;
    }

    return m;
}

void add_castling_moves(const Mailbox& m, Player player, std::vector<Move>& moves)
{
    const static auto king_initial_position = Position{(player == Player::white ? "E1" : "E8")};
    const auto king_square = m.square(king_initial_position);

    if ((player == Player::white && king_square != Square::white_king) ||
        (player == Player::black && king_square != Square::black_king))
    {
        return;
    }

    if (in_check(m, player)) return;

    // Check Queenside
    [&] {
        if (!is_empty(m.square(king_initial_position - 1))) return;
        if (!is_empty(m.square(king_initial_position - 2))) return;
        if (!is_empty(m.square(king_initial_position - 3))) return;

        const auto rook_square = m.square(king_initial_position - 4);

        if ((player == Player::white && rook_square != Square::white_rook) ||
            (player == Player::black && rook_square != Square::black_rook))
        {
            return;
        }

        auto m_copy = m;
        m_copy.apply(Move{king_initial_position, king_initial_position - 1});
        if (in_check(m_copy, player)) return;

        moves.push_back(Move{king_initial_position, king_initial_position - 2});
    }();

    // Check Kingside
    [&] {
        if (!is_empty(m.square(king_initial_position + 1))) return;
        if (!is_empty(m.square(king_initial_position + 2))) return;

        const auto rook_square = m.square(king_initial_position + 3);

        if ((player == Player::white && rook_square != Square::white_rook) ||
            (player == Player::black && rook_square != Square::black_rook))
        {
            return;
        }

        auto m_copy = m;
        m_copy.apply(Move{king_initial_position, king_initial_position + 1});
        if (in_check(m_copy, player)) return;

        moves.push_back(Move{king_initial_position, king_initial_position + 2});
    }();
}

std::vector<Move> generate_moves(const Mailbox& m, Player player)
{
    const auto opponent = opponent_of(player);
    std::vector<Move> moves;

    for (Position i = 0; i < m.size; ++i) {
        const auto square = m.square(i);

        if (is_square_owned_by_player(square, player)) {
            const auto add_linear_moves = [&](int offset) {
                Position destination = i + offset;
                while (is_empty(m.square(destination)) ||
                       is_square_owned_by_player(m.square(destination), opponent))
                {
                    moves.push_back(Move{i, destination});
                    if (is_square_owned_by_player(m.square(destination), opponent)) break;
                    destination += offset;
                }
            };
            const auto add_direct_move = [&](int offset) {
                Position destination = i + offset;
                if (is_empty(m.square(destination)) ||
                    is_square_owned_by_player(m.square(destination), opponent))
                {
                    moves.push_back(Move{i, destination});
                }
            };

            const auto piece = to_piece(square);

            switch (piece) {
            case Piece::empty:
                break;
            case Piece::pawn: {
                // Forward direction for player
                int dir = (player == Player::white ? 1 : -1);

                if (is_empty(m.square(i + dir * 10))) {
                    moves.push_back(Move{i, i + dir * 10});

                    // If the square in front is empty then consider moving two squares (but only
                    // if we're on the right row)
                    if (((player == Player::white && i < 39 && i > 30) ||
                         (player == Player::black && i < 89 && i > 80)) &&
                        is_empty(m.square(i + dir * 20)))
                    {
                        moves.push_back(Move{i, i + dir * 20});
                    }
                }

                auto add_pawn_take_move = [&](int offset) {
                    const Position destination = i + offset;
                    if (is_square_owned_by_player(m.square(destination), opponent)) {
                        moves.push_back(Move{i, destination});
                    }
                };
                add_pawn_take_move(dir * 9);
                add_pawn_take_move(dir * 11);
                break;
            }
            case Piece::rook: {
                add_linear_moves(10);
                add_linear_moves(1);
                add_linear_moves(-1);
                add_linear_moves(-10);
                break;
            }
            case Piece::knight: {
                add_direct_move(21);
                add_direct_move(12);
                add_direct_move(-8);
                add_direct_move(-19);
                add_direct_move(-21);
                add_direct_move(-12);
                add_direct_move(8);
                add_direct_move(19);
                break;
            }
            case Piece::bishop: {
                add_linear_moves(11);
                add_linear_moves(-9);
                add_linear_moves(-11);
                add_linear_moves(9);
                break;
            }
            case Piece::queen: {
                add_linear_moves(10);
                add_linear_moves(1);
                add_linear_moves(-1);
                add_linear_moves(-10);
                add_linear_moves(11);
                add_linear_moves(-9);
                add_linear_moves(-11);
                add_linear_moves(9);
                break;
            }
            case Piece::king: {
                add_direct_move(1);
                add_direct_move(9);
                add_direct_move(10);
                add_direct_move(11);
                add_direct_move(-1);
                add_direct_move(-9);
                add_direct_move(-10);
                add_direct_move(-11);

                break;
            }
            }
        }
    }

    add_castling_moves(m, player, moves);

    return moves;
}

void Mailbox::apply(const Move& m)
{
    // If this is a castling move, then move the rook into place
    if (to_piece(square(m.begin)) == Piece::king &&
        std::abs(static_cast<int>(m.end) - static_cast<int>(m.begin)) == 2)
    {
        const bool kingside = m.end > m.begin;

        auto rook_finishing_position = Position{(kingside ? 5 : 3), m.begin.row()};
        auto rook_starting_position  = Position{(kingside ? 7 : 0), m.begin.row()};

        auto& rook_square = square_array[static_cast<int>(rook_starting_position)];
        rook_square = remove_castling_rights(rook_square);

        this->apply(Move{rook_starting_position, rook_finishing_position});
    }

    auto& square_to_be_moved = square_array[static_cast<int>(m.begin)];
    square_to_be_moved = remove_castling_rights(square_to_be_moved);

    square_array[static_cast<int>(m.end)]   = square(m.begin);
    square_array[static_cast<int>(m.begin)] = Square::empty;

    // Apply pawn promotion to queen
    if (to_piece(this->square(m.end)) == Piece::pawn && (m.end.row() == 0 || m.end.row() == 7)) {
        square_array[static_cast<int>(m.end)] = promote_to(this->square(m.end), Piece::queen);
    }
}

bool in_check(const Mailbox& m, Player p)
{
    const Position king_position = [&] {
        for (Position i = 0; i < m.size; ++i) {
            if (to_piece(m.square(i)) == Piece::king &&
                is_square_owned_by_player(m.square(i), p)) {
                return i;
            }
        }
        // No king???
        assert(false && "No king found in mailbox");
    }();

    const auto moves = generate_moves(m, opponent_of(p));

    for (const auto& move : moves) {
        if (move.end == king_position) return true;
    }
    return false;
}

