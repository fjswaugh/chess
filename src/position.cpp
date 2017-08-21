#include "chess/position.h"
#include "chess/misc.h"

namespace Chess {

namespace {

    std::string::const_iterator parse_fen_to_mailbox(std::string::const_iterator begin,
                                                     std::string::const_iterator end,
                                                     Mailbox& mailbox)
    {
        auto it = begin;

        for (int index = 56; it != end && index >= 0; ++it) {
            char ch = *it;

            if (ch == '/') continue;

            if (ch > '0' && ch <= '9') {
                index += (ch - '0');
            } else {
                mailbox[index] = to_square(ch);
                ++index;
            }

            if (index % 8 == 0) index -= 16;
        }

        return it;
    }

    std::string::const_iterator parse_rest_of_fen(std::string::const_iterator begin,
                                                  std::string::const_iterator end,
                                                  Position& position)
    {
        auto it = begin;

        const auto advance = [&it, end]() -> char {
            assert(it != end);
            char ch = *it;
            ++it;
            return ch;
        };
        const auto expect = [&it, end](char ch) {
            assert(it != end);
            assert(*it == ch);
            ++it;
        };

        expect(' ');

        position.active_player = (advance() == 'w' ? Player::white : Player::black);

        expect(' ');

        // For starters, set castling rights to none
        position.castling[0] = Castle_rights::none;
        position.castling[1] = Castle_rights::none;
        for (char ch = advance(); ch != ' '; ch = advance()) {
            switch (ch) {
            case 'K':
                position.castling[0] =
                    add_castle_rights(position.castling[0], Castle_rights::kingside);
                break;
            case 'k':
                position.castling[1] =
                    add_castle_rights(position.castling[1], Castle_rights::kingside);
                break;
            case 'Q':
                position.castling[0] =
                    add_castle_rights(position.castling[0], Castle_rights::queenside);
                break;
            case 'q':
                position.castling[1] =
                    add_castle_rights(position.castling[1], Castle_rights::queenside);
                break;
            default:
                break;
            }
        }

        {
            const char ch1 = advance();
            if (ch1 != '-') {
                const char ch2 = advance();
                position.en_passant_target = Location(ch1 - 'a', ch2 - '1');
            }
        }

        expect(' ');

        {
            const auto num_begin = it;
            while (advance() != ' ');
            const auto num_end = it;
            position.halfmove_clock = std::stoi(std::string(num_begin, num_end));
        }

        position.fullmove_number = std::stoi(std::string(it, end));

        return end;
    }

    void fill_bitboards_from_mailbox(Position& position)
    {
        for (int i = 0; i < 64; ++i) {
            const Bitboard mask = mask_of(i);
            const Square square = position.mailbox[i];

            position.bitboard_by_square[*square] |= mask;
        }

        const auto& bbs = position.bitboard_by_square;
        position.bitboard_by_player[0] = bbs[0] | bbs[2] | bbs[4] | bbs[6] | bbs[8] | bbs[10];
        position.bitboard_by_player[1] = bbs[1] | bbs[3] | bbs[5] | bbs[7] | bbs[9] | bbs[11];
    }

    void mailbox_to_fen(const Mailbox& mailbox, std::string& fen)
    {
        for (int row = 7; row >= 0; --row) {
            int count = 0;
            for (int col = 0; col <= 7; ++col) {
                const Square square = mailbox[Location(col, row)];
                if (square == Square::empty) {
                    ++count;
                } else {
                    if (count > 0) {
                        fen.push_back(count + '0');
                        count = 0;
                    }
                    fen.push_back(to_char(square));
                }
            }
            if (count > 0) fen.push_back(count + '0');
            fen.push_back('/');
        }
        fen.pop_back();
    }

    void rest_of_position_to_fen(const Position& position, std::string& fen)
    {
        fen.push_back(' ');
        fen.push_back(position.active_player == Player::white ? 'w' : 'b');
        fen.push_back(' ');

        if (position.castling[0] == Castle_rights::none &&
            position.castling[1] == Castle_rights::none) {
            fen.push_back('-');
        } else {
            if (can_castle_kingside (position.castling[0])) fen.push_back('K');
            if (can_castle_queenside(position.castling[0])) fen.push_back('Q');
            if (can_castle_kingside (position.castling[1])) fen.push_back('k');
            if (can_castle_queenside(position.castling[1])) fen.push_back('q');
        }

        fen.push_back(' ');

        if (position.en_passant_target == "a1") {
            fen.push_back('-');
        } else {
            fen += to_string(position.en_passant_target);
        }

        fen.push_back(' ');
        fen += std::to_string(position.halfmove_clock);
        fen.push_back(' ');
        fen += std::to_string(position.fullmove_number);
    }

}

std::istream& operator>>(std::istream& is, Position& position)
{
    // Stupid solution but should work for now
    std::string fen_str;
    std::string tmp;
    for (int i = 0; i < 6; ++i) {
        is >> tmp;
        fen_str += tmp + " ";
    }
    fen_str.pop_back();  // Get rid of that last space
    position = Position::from_fen(fen_str);
    return is;
}

Position Position::from_fen(const std::string& fen_str)
{
    Position position;

    auto it = parse_fen_to_mailbox(fen_str.begin(), fen_str.end(), position.mailbox);
    if (it != fen_str.end()) {
        parse_rest_of_fen(it, fen_str.end(), position);
    }
    fill_bitboards_from_mailbox(position);

    return position;
}

std::string to_fen(const Position& position)
{
    std::string fen;

    mailbox_to_fen(position.mailbox, fen);   
    rest_of_position_to_fen(position, fen);

    return fen;
}

Position apply(Move move, Position position)
{
    constexpr Location rook_locations_for_player[2][2] = {{"a1", "h1"}, {"a8", "h8"}};
    constexpr Location king_location_for_player[2] = {"e1", "e8"};
    constexpr int forward_vector_for_player[2] = {8, -8};

    const Player player = position.active_player;
    const Player opponent = opponent_of(player);

    const Location from      = move.from();
    const Location to        = move.to();
    const Bitboard from_mask = mask_of(from);
    const Bitboard to_mask   = mask_of(to);
    const Square from_square = position.mailbox[from];
    const Square to_square   = position.mailbox[to];

// Move the pieces involved

    position.bitboard_by_square[*from_square] &= ~from_mask;  // Remove from square at old location
    position.bitboard_by_square[*to_square]   &= ~to_mask;    // Remove to square at new location
    position.bitboard_by_square[*from_square] |= to_mask;     // Move from square at new location

    position.mailbox[to] = from_square;
    position.mailbox[from] = Square::empty;

// Reset the en passant target

    position.en_passant_target = "a1";

// Check if we have to remove castling rights

    if (from == rook_locations_for_player[*player][0]) {
        position.castling[*player] =
            Castle_rights(*position.castling[*player] & ~*Castle_rights::queenside);
    }

    if (from == rook_locations_for_player[*player][1]) {
        position.castling[*player] =
            Castle_rights(*position.castling[*player] & ~*Castle_rights::kingside);
    }

    if (from == king_location_for_player[*player]) {
        position.castling[*player] = Castle_rights::none;
    }

// Deal with move information

    if (is_promotion(move.info())) {
        const Square promotion_square = Square(*promotion_piece(move.info()) | *player);

        position.mailbox[to] = promotion_square;

        position.bitboard_by_square[*from_square] &= ~to_mask;
        position.bitboard_by_square[*promotion_square] |= to_mask;
    }

    if (move.info() == Move::Info::double_pawn_push) {
        position.en_passant_target = to - forward_vector_for_player[*player];
    }

    if (move.info() == Move::Info::kingside_castle) {
        position.mailbox[to - 1] = Square(*Piece::rook | *player);
        position.mailbox[rook_locations_for_player[*player][1]] = Square::empty;

        position.bitboard_by_square[*Piece::rook | *player] |= mask_of(to - 1);
        position.bitboard_by_square[*Piece::rook | *player] &=
            ~mask_of(rook_locations_for_player[*player][1]);
    }

    if (move.info() == Move::Info::queenside_castle) {
        position.mailbox[to + 1] = Square(*Piece::rook | *player);
        position.mailbox[rook_locations_for_player[*player][0]] = Square::empty;

        position.bitboard_by_square[*Piece::rook | *player] |= mask_of(to + 1);
        position.bitboard_by_square[*Piece::rook | *player] &=
            ~mask_of(rook_locations_for_player[*player][0]);
    }

    if (move.info() == Move::Info::en_passant_capture) {
        const Location location_of_taken_pawn = to - forward_vector_for_player[*player];

        position.mailbox[location_of_taken_pawn] = Square::empty;

        position.bitboard_by_square[*Piece::pawn | *opponent] &= ~mask_of(location_of_taken_pawn);
    }

// Update other bitboards

    const auto& bbs = position.bitboard_by_square;
    position.bitboard_by_player[0] = bbs[0] | bbs[2] | bbs[4] | bbs[6] | bbs[8] | bbs[10];
    position.bitboard_by_player[1] = bbs[1] | bbs[3] | bbs[5] | bbs[7] | bbs[9] | bbs[11];

// Update other pieces of information

    if (player == Player::black) position.fullmove_number++;

    // Do something to the halfmove clock

    position.active_player = opponent;

    return position;
}

}  // namespace Chess

