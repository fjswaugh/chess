#include "chess/generate_moves.h"
#include "chess/position.h"
#include "chess/move_list.h"

#include "lookup_tables.h"

namespace Chess {

constexpr const Bitboard row[8] = {0x00000000000000FF, 0x000000000000FF00, 0x0000000000FF0000,
                                   0x00000000FF000000, 0x000000FF00000000, 0x0000FF0000000000,
                                   0x00FF000000000000, 0xFF00000000000000};

constexpr const Bitboard col[8] = {0x0101010101010101, 0x0202020202020202, 0x0404040404040404,
                                   0x0808080808080808, 0x1010101010101010, 0x2020202020202020,
                                   0x4040404040404040, 0x8080808080808080};

constexpr const Bitboard fdiag[64] = {
    0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008,
    0x0000000080402010, 0x0000000000804020, 0x0000000000008040, 0x0000000000000080,
    0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804,
    0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 0x0000000000008040,
    0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402,
    0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 0x0000000000804020,
    0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201,
    0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010,
    0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100,
    0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008,
    0x0402010000000000, 0x0804020100000000, 0x1008040201000000, 0x2010080402010000,
    0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804,
    0x0201000000000000, 0x0402010000000000, 0x0804020100000000, 0x1008040201000000,
    0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402,
    0x0100000000000000, 0x0201000000000000, 0x0402010000000000, 0x0804020100000000,
    0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201,
};

constexpr const Bitboard rdiag[64] = {
    0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408,
    0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
    0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810,
    0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000,
    0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020,
    0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000,
    0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040,
    0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000,
    0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
    0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000,
    0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000,
    0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000,
    0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000,
    0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000,
    0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000,
    0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
};

u8 inline bit_scan_forward(Bitboard i) { return __builtin_ffsll(i) - 1; }

constexpr Bitboard rotate_left(Bitboard x, unsigned n)
{
    const unsigned int mask = 63;

    assert((n <= mask) && "rotate by type width or more");

    n &= mask;
    return (x << n) | (x >> ((-n) & mask));
}

// Lookup moves functions
namespace {
    template <Piece>
    Bitboard lookup_moves(Location, Bitboard);

    template <Piece>
    Bitboard lookup_moves(Location);

    template <>
    Bitboard lookup_moves<Piece::rook>(Location l, Bitboard occupancy_board)
    {
        const Bitboard blockers = attack_lookup_table<Piece::rook>[l] & occupancy_board;
        const Bitboard magic_board = magic_bitboard_lookup_table<Piece::rook>[l];

        constexpr int index_size = 12;
        const int index = (blockers * magic_board) >> (64 - index_size);

        return move_lookup_table<Piece::rook>[l][index];
    }

    template <>
    Bitboard lookup_moves<Piece::knight>(Location l)
    {
        return move_lookup_table<Piece::knight>[l];
    }

    template <>
    Bitboard lookup_moves<Piece::bishop>(Location l, Bitboard occupancy_board)
    {
        const Bitboard blockers = attack_lookup_table<Piece::bishop>[l] & occupancy_board;
        const Bitboard magic_board = magic_bitboard_lookup_table<Piece::bishop>[l];

        constexpr int index_size = 9;
        const int index = (blockers * magic_board) >> (64 - index_size);

        return move_lookup_table<Piece::bishop>[l][index];
    }

    template <>
    Bitboard lookup_moves<Piece::queen>(Location l, Bitboard occupancy_board)
    {
        return lookup_moves<Piece::rook>(l, occupancy_board) |
               lookup_moves<Piece::bishop>(l, occupancy_board);
    }

    template <>
    Bitboard lookup_moves<Piece::king>(Location l)
    {
        return move_lookup_table<Piece::king>[l];
    }
}

// Extra information about position
namespace {

struct Extra {
    Extra() { std::fill_n(pin_rays, 64, 0xFFFFFFFFFFFFFFFF); }

    Bitboard attacked_by_opponent = 0;
    Bitboard giving_check         = 0;
    Bitboard non_king_move_restriction = 0xFFFFFFFFFFFFFFFF;
    Bitboard pawn_move_restriction     = 0xFFFFFFFFFFFFFFFF;
    Bitboard pin_rays[64];
};

bool in_check(const Position& position, Player player)
{
    const Player opponent = opponent_of(player);

    constexpr static const unsigned right_move_vector_for[2] = {9, 64-7};
    constexpr static const unsigned left_move_vector_for[2]  = {7, 64-9};

    const Bitboard occupancy_board =
        (position.bitboard_by_player[0] | position.bitboard_by_player[1]);

    const Bitboard opponent_rooks   = position.bitboard_by_square[*opponent | *Piece::rook];
    const Bitboard opponent_knights = position.bitboard_by_square[*opponent | *Piece::knight];
    const Bitboard opponent_bishops = position.bitboard_by_square[*opponent | *Piece::bishop];
    const Bitboard opponent_queens  = position.bitboard_by_square[*opponent | *Piece::queen];
    const Bitboard opponent_king    = position.bitboard_by_square[*opponent | *Piece::king];
    const Bitboard opponent_pawns   = position.bitboard_by_square[*opponent | *Piece::pawn];

    const Bitboard player_king = position.bitboard_by_square[*player | *Piece::king];

    Bitboard attacked = 0;

    attacked |= lookup_moves<Piece::king>(bit_scan_forward(opponent_king));

    for (Bitboard b = opponent_knights; b; b &= (b - 1)) {
        const u8 l = bit_scan_forward(b);
        attacked |= lookup_moves<Piece::knight>(l);
    }

    for (Bitboard b = opponent_rooks; b; b &= (b - 1)) {
        const u8 l = bit_scan_forward(b);
        attacked |= lookup_moves<Piece::rook>(l, occupancy_board);
    }

    for (Bitboard b = opponent_bishops; b; b &= (b - 1)) {
        const u8 l = bit_scan_forward(b);
        attacked |= lookup_moves<Piece::bishop>(l, occupancy_board);
    }

    for (Bitboard b = opponent_queens; b; b &= (b - 1)) {
        const u8 l = bit_scan_forward(b);
        attacked |= lookup_moves<Piece::queen>(l, occupancy_board);
    }

    const unsigned left_move_vector  = left_move_vector_for[*opponent];
    const unsigned right_move_vector = right_move_vector_for[*opponent];

    const Bitboard left_targets  = rotate_left(opponent_pawns, left_move_vector)  & ~col[7];
    const Bitboard right_targets = rotate_left(opponent_pawns, right_move_vector) & ~col[0];

    attacked |= (left_targets | right_targets);

    return attacked & player_king;
}

Extra generate_extra_information(const Position& position)
{
    const Player player   = position.active_player;
    const Player opponent = opponent_of(player);

    constexpr static const unsigned right_move_vector_for[2] = {9, 64-7};
    constexpr static const unsigned left_move_vector_for[2]  = {7, 64-9};

    const Bitboard occupancy_board =
        (position.bitboard_by_player[0] | position.bitboard_by_player[1]);

    const Bitboard opponent_rooks   = position.bitboard_by_square[*opponent | *Piece::rook];
    const Bitboard opponent_knights = position.bitboard_by_square[*opponent | *Piece::knight];
    const Bitboard opponent_bishops = position.bitboard_by_square[*opponent | *Piece::bishop];
    const Bitboard opponent_queens  = position.bitboard_by_square[*opponent | *Piece::queen];
    const Bitboard opponent_king    = position.bitboard_by_square[*opponent | *Piece::king];
    const Bitboard opponent_pawns   = position.bitboard_by_square[*opponent | *Piece::pawn];

    const Bitboard player_king = position.bitboard_by_square[*player | *Piece::king];

    Extra e;

    {
        // Place each enemy piece in player's king location and see what it can hit (apart from
        // enemy king, since they are incapable of giving check)

        const Location kl = bit_scan_forward(player_king);

        e.giving_check = (lookup_moves<Piece::knight>(kl                 ) & opponent_knights) |
                         (lookup_moves<Piece::rook  >(kl, occupancy_board) & opponent_rooks)   |
                         (lookup_moves<Piece::bishop>(kl, occupancy_board) & opponent_bishops) |
                         (lookup_moves<Piece::queen >(kl, occupancy_board) & opponent_queens);

        const unsigned left_move_vector  = left_move_vector_for[*player];
        const unsigned right_move_vector = right_move_vector_for[*player];

        const Bitboard left_targets  = rotate_left(player_king, left_move_vector)  & ~col[7];
        const Bitboard right_targets = rotate_left(player_king, right_move_vector) & ~col[0];

        e.giving_check |= (left_targets | right_targets) & opponent_pawns;
    }

    {
        // Go through each piece and 'or' its move bitboards to get the total attacked locations

        e.attacked_by_opponent |= lookup_moves<Piece::king>(bit_scan_forward(opponent_king));

        for (Bitboard b = opponent_knights; b; b &= (b - 1)) {
            const u8 l = bit_scan_forward(b);
            e.attacked_by_opponent |= lookup_moves<Piece::knight>(l);
        }

        const Bitboard occupancy_minus_king = occupancy_board & ~player_king;

        for (Bitboard b = opponent_rooks; b; b &= (b - 1)) {
            const u8 l = bit_scan_forward(b);
            e.attacked_by_opponent |= lookup_moves<Piece::rook>(l, occupancy_minus_king);
        }

        for (Bitboard b = opponent_bishops; b; b &= (b - 1)) {
            const u8 l = bit_scan_forward(b);
            e.attacked_by_opponent |= lookup_moves<Piece::bishop>(l, occupancy_minus_king);
        }

        for (Bitboard b = opponent_queens; b; b &= (b - 1)) {
            const u8 l = bit_scan_forward(b);
            e.attacked_by_opponent |= lookup_moves<Piece::queen>(l, occupancy_minus_king);
        }

        const unsigned left_move_vector  = left_move_vector_for[*opponent];
        const unsigned right_move_vector = right_move_vector_for[*opponent];

        const Bitboard left_targets  = rotate_left(opponent_pawns, left_move_vector)  & ~col[7];
        const Bitboard right_targets = rotate_left(opponent_pawns, right_move_vector) & ~col[0];

        e.attacked_by_opponent |= (left_targets | right_targets);
    }

    const auto locations_inbetween = [](Location l1, Location l2) -> Bitboard {
        Location bigger  = l1 < l2 ? l2 : l1;
        Location smaller = l1 < l2 ? l1 : l2;

        Bitboard b = 0;

        if (l1.row() == l2.row()) {
            bigger = bigger - 1;
            while (bigger > smaller) {
                b |= mask_of(bigger);
                bigger = bigger - 1;
            }
        } else if (l1.col() == l2.col()) {
            bigger = bigger - 8;
            while (bigger > smaller) {
                b |= mask_of(bigger);
                bigger = bigger - 8;
            }
        } else if (fdiag[l1] == fdiag[l2]) {
            bigger = bigger - 9;
            while (bigger > smaller) {
                b |= mask_of(bigger);
                bigger = bigger - 9;
            }
        } else if (rdiag[l1] == rdiag[l2]) {
            bigger = bigger - 7;
            while (bigger > smaller) {
                b |= mask_of(bigger);
                bigger = bigger - 7;
            }
        } else {
            return 0;
        }

        return b;
    };

    {
        // If in single check, make some restrictions
        if (count(e.giving_check) == 1) {
            // Normal pieces can only move to take those giving check...
            e.non_king_move_restriction = e.giving_check;

            const Location threat_location = bit_scan_forward(e.giving_check);
            const Piece threat_piece = to_piece(position.mailbox[threat_location]);

            if (threat_piece == Piece::rook || threat_piece == Piece::bishop ||
                threat_piece == Piece::queen)
            {
                // ...or in the case of sliding pieces, block them
                e.non_king_move_restriction |=
                    locations_inbetween(threat_location, bit_scan_forward(player_king));
            }

            // Pawns may move the same way, but if there is an en passant square, then it will also
            // be legal for them to move there
            e.pawn_move_restriction = e.non_king_move_restriction;
            if (position.en_passant_target != "a1") {
                e.pawn_move_restriction |= mask_of(position.en_passant_target);
            }
        }

        // If in double check, no piece other than the king can move
        if (count(e.giving_check) > 1) {
            e.non_king_move_restriction = 0;
            e.pawn_move_restriction = 0;
        }
    }

    { // Generate pin rays
        const Location kl = bit_scan_forward(player_king);

        const Bitboard king_rays = (lookup_moves<Piece::rook>(kl, occupancy_board)) |
                                   (lookup_moves<Piece::bishop>(kl, occupancy_board)) |
                                   (lookup_moves<Piece::queen>(kl, occupancy_board));

        for (Bitboard b = opponent_rooks; b; b &= (b - 1)) {
            const Location l = bit_scan_forward(b);

            const Bitboard rook_moves = lookup_moves<Piece::rook>(l, occupancy_board);

            const u8 c = bit_scan_forward(rook_moves & col[l.col()] & king_rays & col[kl.col()]);
            const u8 r = bit_scan_forward(rook_moves & row[l.row()] & king_rays & row[kl.row()]);
            if (c < 64) e.pin_rays[c] = col[l.col()];
            if (r < 64) e.pin_rays[r] = row[l.row()];
        }

        for (Bitboard b = opponent_bishops; b; b &= (b - 1)) {
            const Location l = bit_scan_forward(b);

            const Bitboard bishop_moves = lookup_moves<Piece::bishop>(l, occupancy_board);

            const u8 fd = bit_scan_forward(bishop_moves & fdiag[l] & king_rays & fdiag[kl]);
            const u8 rd = bit_scan_forward(bishop_moves & rdiag[l] & king_rays & rdiag[kl]);
            if (fd < 64) e.pin_rays[fd] = fdiag[l];
            if (rd < 64) e.pin_rays[rd] = rdiag[l];
        }

        for (Bitboard b = opponent_queens; b; b &= (b - 1)) {
            const Location l = bit_scan_forward(b);

            const Bitboard queen_moves = lookup_moves<Piece::queen>(l, occupancy_board);

            const u8 c  = bit_scan_forward(queen_moves & col[l.col()] & king_rays & col[kl.col()]);
            const u8 r  = bit_scan_forward(queen_moves & row[l.row()] & king_rays & row[kl.row()]);
            const u8 fd = bit_scan_forward(queen_moves & fdiag[l] & king_rays & fdiag[kl]);
            const u8 rd = bit_scan_forward(queen_moves & rdiag[l] & king_rays & rdiag[kl]);
            if (c < 64)  e.pin_rays[c]  = col[l.col()];
            if (r < 64)  e.pin_rays[r]  = row[l.row()];
            if (fd < 64) e.pin_rays[fd] = fdiag[l];
            if (rd < 64) e.pin_rays[rd] = rdiag[l];
        }
    }

    return e;
}

}

// Add moves to move list functions
namespace {

void add_legal_king_moves(u8 begin, Bitboard targets, const Position& position,
                          const Extra& extra, Move_list& moves)
{
    const Player opponent = opponent_of(position.active_player);

    targets &= ~extra.attacked_by_opponent;

    while (targets) {
        const Location end = bit_scan_forward(targets);

        const auto info = is_square_owned_by_player(position.mailbox[end], opponent)
                              ? Move::Info::normal_capture
                              : Move::Info::normal;

        moves.emplace_back(begin, end, info);
        targets &= (targets - 1);
    }
}

void add_legal_castles(const Position& position, const Extra& extra, Move_list& moves)
{
    constexpr Location king_location_for[2] = {"e1", "e8"};
    constexpr Location kingside_destination_for[2] = {"g1", "g8"};
    constexpr Location queenside_destination_for[2] = {"c1", "c8"};
    constexpr Location kingside_rook_location_for[2]  = {"h1", "h8"};
    constexpr Location queenside_rook_location_for[2] = {"a1", "a8"};
    constexpr Bitboard kingside_inbetween_for[2]  = {0x0000000000000060, 0x6000000000000000};
    constexpr Bitboard queenside_inbetween_for[2] = {0x000000000000000E, 0x0E00000000000000};
    constexpr Bitboard kingside_king_movement_for[2]  = {0x0000000000000070, 0x7000000000000000};
    constexpr Bitboard queenside_king_movement_for[2] = {0x000000000000001C, 0x1C00000000000000};

    const Bitboard occupancy_board =
        position.bitboard_by_player[0] | position.bitboard_by_player[1];
    const Player player = position.active_player;

    if (can_castle_kingside(position.castling[*player]) &&
        position.mailbox[kingside_rook_location_for[*player]] == Square(*player | *Piece::rook) &&
        (occupancy_board & kingside_inbetween_for[*player]) == 0 &&
        (extra.attacked_by_opponent & kingside_king_movement_for[*player]) == 0)
    {
        moves.emplace_back(king_location_for[*player], kingside_destination_for[*player],
                           Move::Info::kingside_castle);
    }

    if (can_castle_queenside(position.castling[*player]) &&
        position.mailbox[queenside_rook_location_for[*player]] == Square(*player | *Piece::rook) &&
        (occupancy_board & queenside_inbetween_for[*player]) == 0 &&
        (extra.attacked_by_opponent & queenside_king_movement_for[*player]) == 0)
    {
        moves.emplace_back(king_location_for[*player], queenside_destination_for[*player],
                           Move::Info::queenside_castle);
    }
}

void add_legal_non_king_moves(u8 begin, Bitboard targets, const Position& position,
                              const Extra& extra, Move_list& moves)
{
    const Player opponent = opponent_of(position.active_player);

    targets &= extra.non_king_move_restriction;
    targets &= extra.pin_rays[begin];

    while (targets) {
        const Location end = bit_scan_forward(targets);

        const auto info = is_square_owned_by_player(position.mailbox[end], opponent)
                              ? Move::Info::normal_capture
                              : Move::Info::normal;

        moves.emplace_back(begin, end, info);
        targets &= (targets - 1);
    }
}

void add_legal_pawn_moves(unsigned vector, Bitboard targets, const Extra& extra, Move::Info info,
                          Move_list& moves)
{
    targets &= extra.pawn_move_restriction;

    while (targets) {
        const Location end = bit_scan_forward(targets);
        const Location begin = u8(end - vector) % 64;

        if (mask_of(end) & extra.pin_rays[begin]) {
            moves.emplace_back(begin, end, info);
        }

        targets &= (targets - 1);
    }
}

void add_legal_pawn_promotions(unsigned vector, Bitboard targets, const Extra& extra,
                               Move::Info info, Move_list& moves)
{
    targets &= extra.pawn_move_restriction;

    info = Move::Info(*info & 0b00001100);

    while (targets) {
        const Location end = bit_scan_forward(targets);
        const Location begin = u8(end - vector) % 64;

        if (mask_of(end) & extra.pin_rays[begin]) {
            moves.emplace_back(begin, end, Move::Info(*info | 0b00000000));
            moves.emplace_back(begin, end, Move::Info(*info | 0b00000001));
            moves.emplace_back(begin, end, Move::Info(*info | 0b00000010));
            moves.emplace_back(begin, end, Move::Info(*info | 0b00000011));
        }

        targets &= (targets - 1);
    }
}

}

// Move generation functions
namespace {

void generate_king_moves(const Position& position, const Extra& extra, Move_list& moves)
{
    const auto player = position.active_player;
    Bitboard kings = position.bitboard_by_square[*player | *Piece::king];

    const Bitboard possible_targets = ~position.bitboard_by_player[*player];

    assert(kings && "There must be a king on the board, always");

    const u8 begin = bit_scan_forward(kings);
    const Bitboard targets = lookup_moves<Piece::king>(begin) & possible_targets;
    add_legal_king_moves(begin, targets, position, extra, moves);

    add_legal_castles(position, extra, moves);
}

void generate_knight_moves(const Position& position, const Extra& extra, Move_list& moves)
{
    const auto player = position.active_player;
    Bitboard knights = position.bitboard_by_square[*player | *Piece::knight];

    const Bitboard possible_targets = ~position.bitboard_by_player[*player];

    while (knights) {
        const u8 begin = bit_scan_forward(knights);
        const Bitboard targets = lookup_moves<Piece::knight>(begin) & possible_targets;
        add_legal_non_king_moves(begin, targets, position, extra, moves);
        knights &= (knights - 1);
    }
}

template <Piece piece,
          typename = std::enable_if_t<piece == Piece::rook || piece == Piece::bishop ||
                                      piece == Piece::queen>>
void generate_sliding_moves(const Position& position, const Extra& extra, Move_list& moves)
{
    const auto player = position.active_player;
    Bitboard pieces = position.bitboard_by_square[*player | *piece];

    const Bitboard possible_targets = ~position.bitboard_by_player[*player];

    const Bitboard occupancy_board =
        position.bitboard_by_player[0] | position.bitboard_by_player[1];

    while (pieces) {
        const u8 begin = bit_scan_forward(pieces);
        const Bitboard targets = lookup_moves<piece>(begin, occupancy_board) & possible_targets;
        add_legal_non_king_moves(begin, targets, position, extra, moves);
        pieces &= (pieces - 1);
    }
}

void generate_pawn_captures(const Position& position, const Extra& extra, Move_list& moves)
{
    const auto player = position.active_player;

    constexpr static const unsigned right_move_vector_for[2] = {9, 64-7};
    constexpr static const unsigned left_move_vector_for[2]  = {7, 64-9};
    constexpr static const Bitboard promotion_row_for[2]     = {row[7], row[0]};

    const unsigned left_move_vector  = left_move_vector_for[*player];
    const unsigned right_move_vector = right_move_vector_for[*player];
    const Bitboard promotion_row     = promotion_row_for[*player];

    const Bitboard pawns = position.bitboard_by_square[*Piece::pawn | *player];
    const Bitboard left_diagonals  = rotate_left(pawns, left_move_vector)  & ~col[7];
    const Bitboard right_diagonals = rotate_left(pawns, right_move_vector) & ~col[0];

    const Bitboard possible_targets = position.bitboard_by_player[*opponent_of(player)];

    const Bitboard left_targets  = left_diagonals  & possible_targets;
    const Bitboard right_targets = right_diagonals & possible_targets;

    // Action happening here
    add_legal_pawn_moves(left_move_vector, left_targets & ~promotion_row, extra,
                         Move::Info::normal_capture, moves);
    add_legal_pawn_moves(right_move_vector, right_targets & ~promotion_row, extra,
                         Move::Info::normal_capture, moves);
    add_legal_pawn_promotions(left_move_vector, left_targets & promotion_row, extra,
                         Move::Info::promotion_capture, moves);
    add_legal_pawn_promotions(right_move_vector, right_targets & promotion_row, extra,
                         Move::Info::promotion_capture, moves);

    // Test for en passant
    if (position.en_passant_target != "a1") {
        const Bitboard possible_targets = mask_of(position.en_passant_target);

        const Bitboard left_targets  = left_diagonals  & possible_targets;
        const Bitboard right_targets = right_diagonals & possible_targets;

        if (left_targets) {
            const Location end = bit_scan_forward(left_targets);
            const Location begin = u8(end - left_move_vector) % 64;
            Move move(begin, end, Move::Info::en_passant_capture);
            const auto new_position = apply(move, position);

            if (!in_check(new_position, player)) {
                add_legal_pawn_moves(left_move_vector, left_targets, extra,
                                     Move::Info::en_passant_capture, moves);
            }
        }

        if (right_targets) {
            const Location end = bit_scan_forward(right_targets);
            const Location begin = u8(end - right_move_vector) % 64;
            Move move(begin, end, Move::Info::en_passant_capture);
            const auto new_position = apply(move, position);

            if (!in_check(new_position, player)) {
                add_legal_pawn_moves(right_move_vector, right_targets, extra,
                                     Move::Info::en_passant_capture, moves);
            }
        }
    }
}

void generate_pawn_pushes(const Position& position, const Extra& extra, Move_list& moves)
{
    const auto player = position.active_player;

    constexpr static const unsigned move_vector_for[2]   = {8, 64-8};
    constexpr static const Bitboard third_row_for[2]     = {row[2], row[5]};
    constexpr static const Bitboard promotion_row_for[2] = {row[7], row[0]};

    const unsigned move_vector   = move_vector_for[*player];
    const Bitboard third_row     = third_row_for[*player];
    const Bitboard promotion_row = promotion_row_for[*player];

    const Bitboard pawns = position.bitboard_by_square[*Piece::pawn | *player];
    const Bitboard empty = ~(position.bitboard_by_player[0] | position.bitboard_by_player[1]);

    const Bitboard single_push_targets =
        rotate_left(pawns, move_vector) & empty & ~promotion_row;

    const Bitboard double_push_targets =
        rotate_left(single_push_targets & third_row, move_vector) & empty;

    const Bitboard promotion_targets =
        rotate_left(pawns, move_vector) & empty & promotion_row;

    // Action happening here
    add_legal_pawn_moves(move_vector, single_push_targets, extra, Move::Info::normal, moves);
    add_legal_pawn_moves(move_vector * 2, double_push_targets, extra, Move::Info::double_pawn_push,
                         moves);
    add_legal_pawn_promotions(move_vector, promotion_targets, extra, Move::Info::normal_promotion,
                              moves);
}

}

Move_list generate_moves(const Position& position)
{
    Move_list moves;

    const Extra e = generate_extra_information(position);

    generate_king_moves(position, e, moves);

    generate_pawn_pushes(position, e, moves);
    generate_pawn_captures(position, e, moves);

    generate_knight_moves(position, e, moves);
    generate_sliding_moves<Piece::rook  >(position, e, moves);
    generate_sliding_moves<Piece::bishop>(position, e, moves);
    generate_sliding_moves<Piece::queen >(position, e, moves);

    return moves;
}

bool is_legal_move(Move move, const Position& position)
{
    const auto moves = generate_moves(position);

    return std::find(moves.begin(), moves.end(), move) != moves.end();
}

}  // namespace Chess

