#pragma once

#include <cstdint>
#include "player.h"

using Underlying_square_t = std::uint8_t;

enum class Square : Underlying_square_t {
    /*
     * Bit 0: white
     * Bit 1: black
     * Bits 2-4: type
     *     0 = empty
     *     1 = pawn
     *     2 = rook
     *     3 = knight
     *     4 = bishop
     *     5 = queen
     *     6 = king
     *     7 = unused
     * Bit 5: en passant target
     * Bit 6: can castle flag
     * Bit 7: off board
     */
    off_board         = 0b00000001,
    empty             = 0b00000000,
    en_passant_target = 0b00000100,

    white_pawn    = 0b10001000,
    white_rook    = 0b10010010,
    white_rook_nc = 0b10010000,  // nc = no castling
    white_knight  = 0b10011000,
    white_bishop  = 0b10100000,
    white_queen   = 0b10101000,
    white_king    = 0b10110010,
    white_king_nc = 0b10110000,  // nc = no castling

    black_pawn    = 0b01001000,
    black_rook    = 0b01010010,
    black_rook_nc = 0b01010000,  // nc = no castling
    black_knight  = 0b01011000,
    black_bishop  = 0b01100000,
    black_queen   = 0b01101000,
    black_king    = 0b01110010,
    black_king_nc = 0b01110000,  // nc = no castling
};

enum class Piece : Underlying_square_t {
    // Same bit pattern as above
    empty  = 0b00000000,
    pawn   = 0b00001000,
    rook   = 0b00010000,
    knight = 0b00011000,
    bishop = 0b00100000,
    queen  = 0b00101000,
    king   = 0b00110000,
};

inline constexpr Underlying_square_t operator"" _ust(unsigned long long x)
{
    return static_cast<Underlying_square_t>(x);
}

inline constexpr Underlying_square_t to_ust(Square s)
{
    return static_cast<Underlying_square_t>(s);
}

inline constexpr Square remove_castling_rights(Square s)
{
    return static_cast<Square>(to_ust(s) & 0b11111101);
}

inline constexpr bool is_empty(Square s) {
    return s == Square::empty || s == Square::en_passant_target;
}

inline constexpr Square promote_to(Square s, Piece p) {
    // Remove castling rights and piece information
    s = static_cast<Square>(to_ust(s) & 0b11000101);

    Underlying_square_t piece_bits = static_cast<Underlying_square_t>(p) & 0b00111000;
    s = static_cast<Square>(to_ust(s) | piece_bits);
    return s;
}

inline constexpr bool can_castle(Square s)
{
    return to_ust(s) & 0b00000010;
}

inline constexpr bool is_white(Square s) {
    return to_ust(s) & 0b10000000;
}

inline constexpr bool is_black(Square s) {
    return to_ust(s) & 0b01000000;
}

inline constexpr bool is_square_owned_by_player(Square s, Player p) {
    return (is_white(s) && p == Player::white) || (is_black(s) && p == Player::black);
}

constexpr inline Piece to_piece(Square s)
{
    return static_cast<Piece>(to_ust(s) & 0b00111000);
}

inline auto to_unicode(const Square s)
{
    switch (s) {
    case Square::white_pawn:
        return "\u2659";
    case Square::white_rook:
    case Square::white_rook_nc:
        return "\u2656";
    case Square::white_knight:
        return "\u2658";
    case Square::white_bishop:
        return "\u2657";
    case Square::white_queen:
        return "\u2655";
    case Square::white_king:
    case Square::white_king_nc:
        return "\u2654";
    case Square::black_pawn:
        return "\u265F";
    case Square::black_rook:
    case Square::black_rook_nc:
        return "\u265C";
    case Square::black_knight:
        return "\u265E";
    case Square::black_bishop:
        return "\u265D";
    case Square::black_queen:
        return "\u265B";
    case Square::black_king:
    case Square::black_king_nc:
        return "\u265A";
    default:
        return " ";
    }
}

inline char to_char(const Square s)
{
    switch (s) {
    case Square::white_pawn:
        return 'P';
    case Square::white_rook:
    case Square::white_rook_nc:
        return 'R';
    case Square::white_knight:
        return 'N';
    case Square::white_bishop:
        return 'B';
    case Square::white_queen:
        return 'Q';
    case Square::white_king:
    case Square::white_king_nc:
        return 'K';
    case Square::black_pawn:
        return 'p';
    case Square::black_rook:
    case Square::black_rook_nc:
        return 'r';
    case Square::black_knight:
        return 'n';
    case Square::black_bishop:
        return 'b';
    case Square::black_queen:
        return 'q';
    case Square::black_king:
    case Square::black_king_nc:
        return 'k';
    default:
        return ' ';
    }
}

inline Square to_square(char ch)
{
    switch (ch) {
    case 'P':
        return Square::white_pawn;
    case 'R':
        return Square::white_rook;
    case 'N':
        return Square::white_knight;
    case 'B':
        return Square::white_bishop;
    case 'Q':
        return Square::white_queen;
    case 'K':
        return Square::white_king;
    case 'p':
        return Square::black_pawn;
    case 'r':
        return Square::black_rook;
    case 'n':
        return Square::black_knight;
    case 'b':
        return Square::black_bishop;
    case 'q':
        return Square::black_queen;
    case 'k':
        return Square::black_king;
    case ' ': default:
        return Square::empty;
    }
}

