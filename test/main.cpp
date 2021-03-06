#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include <string>
#include <sstream>
#include <chrono>

#include "chess/chess.h"
#include "perft.h"
#include "print.h"

using namespace Chess;

const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

BOOST_AUTO_TEST_CASE(bitboard_conversion)
{
    auto position = Position::from_fen(initial_fen);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_rook] ==
                0b0000000000000000000000000000000000000000000000000000000010000001);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_knight] ==
                0b0000000000000000000000000000000000000000000000000000000001000010);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_bishop] ==
                0b0000000000000000000000000000000000000000000000000000000000100100);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_queen] ==
                0b0000000000000000000000000000000000000000000000000000000000001000);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_king] ==
                0b0000000000000000000000000000000000000000000000000000000000010000);
    BOOST_CHECK(position.bitboard_by_square[*Square::white_pawn] ==
                0b0000000000000000000000000000000000000000000000001111111100000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_rook] ==
                0b1000000100000000000000000000000000000000000000000000000000000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_knight] ==
                0b0100001000000000000000000000000000000000000000000000000000000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_bishop] ==
                0b0010010000000000000000000000000000000000000000000000000000000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_queen] ==
                0b0000100000000000000000000000000000000000000000000000000000000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_king] ==
                0b0001000000000000000000000000000000000000000000000000000000000000);
    BOOST_CHECK(position.bitboard_by_square[*Square::black_pawn] ==
                0b0000000011111111000000000000000000000000000000000000000000000000);
}

BOOST_AUTO_TEST_CASE(general_position_tests)
{
    std::cout << "Size of position: " << sizeof(Position) << '\n';

    Position p = Position::from_fen(initial_fen);

    BOOST_CHECK(p.castling[0] == Castle_rights::both && p.castling[1] == Castle_rights::both);
    BOOST_CHECK(p.en_passant_target == "a1");
    BOOST_CHECK(p.active_player == Player::white);
    BOOST_CHECK(p.halfmove_clock == 0);

    const auto fen = to_fen(p);
    BOOST_CHECK(fen == initial_fen);
}

BOOST_AUTO_TEST_CASE(starting_position)
{
    const auto p = Position::from_fen(initial_fen);

    BOOST_CHECK(count_moves(p, 1) == 20);
    BOOST_CHECK(count_moves(p, 2) == 20 * 20);
    BOOST_CHECK(count_moves(p, 3) == 8902);
    BOOST_CHECK(count_moves(p, 4) == 197281);
    BOOST_CHECK(count_moves(p, 5) == 4865609);
    //BOOST_CHECK(count_moves(p, 6) == 119060324);
}

BOOST_AUTO_TEST_CASE(kiwipete)
{
    const auto p =
        Position::from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    BOOST_CHECK(count_moves(p, 1) == 48);
    BOOST_CHECK(count_moves(p, 2) == 2039);
    BOOST_CHECK(count_moves(p, 3) == 97862);
    BOOST_CHECK(count_moves(p, 4) == 4085603);
    BOOST_CHECK(count_moves(p, 5) == 193690690);
}

BOOST_AUTO_TEST_CASE(position_3)
{
    const auto p = Position::from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    BOOST_CHECK(count_moves(p, 1) == 14);
    BOOST_CHECK(count_moves(p, 2) == 191);
    BOOST_CHECK(count_moves(p, 3) == 2812);
    BOOST_CHECK(count_moves(p, 4) == 43238);
    BOOST_CHECK(count_moves(p, 5) == 674624);
    BOOST_CHECK(count_moves(p, 6) == 11030083);
    BOOST_CHECK(count_moves(p, 7) == 178633661);
}

BOOST_AUTO_TEST_CASE(position_4)
{
    const auto p =
        Position::from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    BOOST_CHECK(count_moves(p, 1) == 6);
    BOOST_CHECK(count_moves(p, 2) == 264);
    BOOST_CHECK(count_moves(p, 3) == 9467);
    BOOST_CHECK(count_moves(p, 4) == 422333);
    BOOST_CHECK(count_moves(p, 5) == 15833292);
    //BOOST_CHECK(count_moves(p, 6) == 706045033);
}

BOOST_AUTO_TEST_CASE(position_5)
{
    const auto p = Position::from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

    BOOST_CHECK(count_moves(p, 1) == 44);
    BOOST_CHECK(count_moves(p, 2) == 1486);
    BOOST_CHECK(count_moves(p, 3) == 62379);
    BOOST_CHECK(count_moves(p, 4) == 2103487);
    BOOST_CHECK(count_moves(p, 5) == 89941194);
}

BOOST_AUTO_TEST_CASE(position_6)
{
    const auto p = Position::from_fen(
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"); 
    BOOST_CHECK(count_moves(p, 1) == 46);
    BOOST_CHECK(count_moves(p, 2) == 2079);
    BOOST_CHECK(count_moves(p, 3) == 89890);
    BOOST_CHECK(count_moves(p, 4) == 3894594);
    BOOST_CHECK(count_moves(p, 5) == 164075551);
}

BOOST_AUTO_TEST_CASE(other_perfts)
{
    //--Illegal ep move #1
    BOOST_CHECK(count_moves(Position::from_fen("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1"), 6) ==
                1134888);

    //--Illegal ep move #2
    BOOST_CHECK(count_moves(Position::from_fen("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1"), 6) ==
                1015133);

    //--EP Capture Checks Opponent
    BOOST_CHECK(count_moves(Position::from_fen("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1"), 6) ==
                1440467);

    //--Short Castling Gives Check
    BOOST_CHECK(count_moves(Position::from_fen("5k2/8/8/8/8/8/8/4K2R w K - 0 1"), 6) == 661072);

    //--Long Castling Gives Check
    BOOST_CHECK(count_moves(Position::from_fen("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1"), 6) == 803711);

    //--Castle Rights
    BOOST_CHECK(count_moves(Position::from_fen("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1"), 4) ==
                1274206);

    //--Castling Prevented
    BOOST_CHECK(count_moves(Position::from_fen("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1"), 4) ==
                1720476);

    //--Promote out of Check
    BOOST_CHECK(count_moves(Position::from_fen("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1"), 6) ==
                3821001);

    //--Discovered Check
    BOOST_CHECK(count_moves(Position::from_fen("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1"), 5) ==
                1004658);

    //--Promote to give check
    BOOST_CHECK(count_moves(Position::from_fen("4k3/1P6/8/8/8/8/K7/8 w - - 0 1"), 6) == 217342);

    //--Under Promote to give check
    BOOST_CHECK(count_moves(Position::from_fen("8/P1k5/K7/8/8/8/8/8 w - - 0 1"), 6) == 92683);

    //--Self Stalemate
    BOOST_CHECK(count_moves(Position::from_fen("K1k5/8/P7/8/8/8/8/8 w - - 0 1"), 6) == 2217);

    //--Stalemate & Checkmate
    BOOST_CHECK(count_moves(Position::from_fen("8/k1P5/8/1K6/8/8/8/8 w - - 0 1"), 7) == 567584);

    //--Stalemate & Checkmate
    BOOST_CHECK(count_moves(Position::from_fen("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1"), 4) == 23527);
}

BOOST_AUTO_TEST_CASE(nps)
{
    Position p = Position::from_fen(initial_fen);

    auto a = std::chrono::high_resolution_clock::now();
    const auto x = count_moves(p, 6);
    auto b = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();

    std::cout << x << " in " << time << "ms (" << (1.0 * (double)x) / ((double)time / 1000.0)
              << " nps)\n";
}

BOOST_AUTO_TEST_CASE(deduce_move)
{
    auto p = Position::from_fen(initial_fen);
    auto m = deduce_move_from_coordinates(p, "a2", "a3");
    BOOST_CHECK(m == Move("a2", "a3", Move::Info::normal));
    m = deduce_move_from_coordinates(p, "a2", "a4");
    BOOST_CHECK(m == Move("a2", "a4", Move::Info::double_pawn_push));
}

Move to_move(std::string move_str, const Position& position) {
    const auto from = Chess::Location(&move_str[0]);
    const auto to = Chess::Location(&move_str[2]);
    const auto piece = move_str.length() == 5 ? Chess::to_piece(Chess::to_square(move_str[4]))
                                              : Chess::Piece::empty;
    return Chess::deduce_move_from_coordinates(position, from, to, piece);
}

BOOST_AUTO_TEST_CASE(bug)
{
    auto p = Position::from_fen("Rn6/8/1r6/1PN4p/3P2k1/1R4P1/8/1K6 w - - 0 46");
    auto m = to_move("b1a1", p);
    BOOST_CHECK(m.from() == "b1");
    BOOST_CHECK(m.to() == "a1");
    BOOST_CHECK(m.info() == Move::Info::normal);
    p = apply(to_move("b1a1", p), p);
}

/*
BOOST_AUTO_TEST_CASE(strange_move)
{
    auto p = Position::from_fen("1rb1r3/ppk2pPp/3p4/2b5/5B2/2N5/5PPP/n2K1B1R w - - 2 20");
    Io io;
    auto [move, x] = recommend_move(io, p);
    std::cout << Chess::to_coordinate_string(move) << '\n';
}
*/

