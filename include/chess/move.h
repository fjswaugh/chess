#pragma once

#include "chess/typedefs.h"
#include "chess/misc.h"
#include "chess/location.h"

#include <string>

namespace Chess {

struct Move {
    enum class Info : u8 {
        normal_promotion = 0b00000111,
        rook_promotion   = 0b00000100,
        knight_promotion = 0b00000101,
        bishop_promotion = 0b00000110,
        queen_promotion  = 0b00000111,

        promotion_capture        = 0b00001111,
        rook_promotion_capture   = 0b00001100,
        knight_promotion_capture = 0b00001101,
        bishop_promotion_capture = 0b00001110,
        queen_promotion_capture  = 0b00001111,

        normal            = 0b00000000,
        double_pawn_push  = 0b00000001,
        kingside_castle   = 0b00000010,
        queenside_castle  = 0b00000011,

        normal_capture     = 0b00001000,
        en_passant_capture = 0b00001001,
    };

    constexpr Move(Location from, Location to, Info i = Info::normal)
        : data_((u16(from) << 10) | (u16(to) << 4) | u16(i))
    {}
    constexpr Location from() const {
        return u8((data_ & 0b1111110000000000) >> 10);
    }
    constexpr Location to() const {
        return u8((data_ & 0b0000001111110000) >> 4);
    }
    constexpr Info info() const {
        return Info(data_ & 0b0000000000001111);
    }

    friend constexpr bool operator==(Move a, Move b) {
        return a.data_ == b.data_;
    }
private:
    u16 data_;
};

struct Position;

// If it is a promotion, just promote to queen for now
Move deduce_move_from_coordinates(Location from, Location to, const Position&);

inline constexpr u8 operator*(Move::Info i) {
    return u8(i);
}

inline constexpr bool is_capture(Move::Info i) {
    return *i & 0b00001000;
}

inline constexpr bool is_promotion(Move::Info i) {
    return *i & 0b00000100;
}

inline constexpr Piece promotion_piece(Move::Info i) {
    return Piece((*i & 0b00000011) * 2);
}

inline std::string to_coordinate_string(Move m) {
    return std::string() + to_string(m.from()) + "-" + to_string(m.to());
}

}

