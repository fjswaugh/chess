#pragma once

#include "chess/typedefs.h"

namespace Chess {

enum class Player : u8 {
    white = 0,
    black = 1
};

enum class Piece : u8 {
    rook   = 0,
    knight = 2,
    bishop = 4,
    queen  = 6,
    king   = 8,
    pawn   = 10,
    empty  = 12,
};

enum class Square : u8 {
    white_rook   = 0,
    white_knight = 2,
    white_bishop = 4,
    white_queen  = 6,
    white_king   = 8,
    white_pawn   = 10,

    black_rook   = 1,
    black_knight = 3,
    black_bishop = 5,
    black_queen  = 7,
    black_king   = 9,
    black_pawn   = 11,

    empty        = 12,
};

enum class Castle_rights : u8 {
    none      = 0,
    kingside  = 1,
    queenside = 2,
    both      = 3,
};

// Fancy operator overloading:
// This is the only place I have done something like this
// It is pretty egregious, but it should allow for nicer code
// Basically 'dereferencing' an enum type returns its underlying value as an int
// I wish that C++ would allow this as an implicit conversion, because I still want to use enum
// classes for their other features (scoping, stronger typing than just enum)
inline constexpr int operator*(Player x)        { return int(x); }
inline constexpr int operator*(Piece x)         { return int(x); }
inline constexpr int operator*(Square x)        { return int(x); }
inline constexpr int operator*(Castle_rights x) { return int(x); }

inline constexpr Castle_rights add_castle_rights(Castle_rights a, Castle_rights b) {
    return Castle_rights(*a | *b);
}

inline constexpr Castle_rights remove_castle_rights(Castle_rights rights, Castle_rights from) {
    return Castle_rights(*from & ~(*rights));
}

inline constexpr bool can_castle_kingside(Castle_rights cr) {
    return *cr & 0b00000001;
}

inline constexpr bool can_castle_queenside(Castle_rights cr) {
    return *cr & 0b00000010;
}

inline Player opponent_of(Player p) { return Player(!static_cast<bool>(p)); }

inline constexpr Square promote_to(Square s, Piece p) {
    // Remove piece information
    u8 result = *s & 0b00000001;

    // Add in new piece information
    result |= *p;

    return Square(result);
}

inline constexpr bool is_white(Square s) {
    return (*s & 0b00000001) == 0;
}

inline constexpr bool is_black(Square s) {
    return (*s & 0b00000001) == 1;
}

inline constexpr bool is_square_owned_by_player(Square s, Player p) {
    return (*s & 0b00000001) == *p;
}

inline constexpr Piece to_piece(Square s)
{
    return Piece(*s & 0b11111110);
}

inline auto to_unicode(const Square s)
{
    switch (s) {
    case Square::white_pawn:
        return "\u2659";
    case Square::white_rook:
        return "\u2656";
    case Square::white_knight:
        return "\u2658";
    case Square::white_bishop:
        return "\u2657";
    case Square::white_queen:
        return "\u2655";
    case Square::white_king:
        return "\u2654";
    case Square::black_pawn:
        return "\u265F";
    case Square::black_rook:
        return "\u265C";
    case Square::black_knight:
        return "\u265E";
    case Square::black_bishop:
        return "\u265D";
    case Square::black_queen:
        return "\u265B";
    case Square::black_king:
        return "\u265A";
    default:
        return " ";
    }
}

inline char to_char(const Square s)
{
    static constexpr const char string[] = "RrNnBbQqKkPp ";
    return string[*s];
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

}  // namespace Chess

