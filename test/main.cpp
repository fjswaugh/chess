#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include <chrono>

#include "chess/chess.h"
#include "rang.hpp"

void pretty_print(const Chess::Board& b)
{
    for (auto row = 8; row-- > 0;) {
        for (auto col = 0; col < 8; ++col) {
            if ((row + col) % 2 == 0) {
                std::cout << rang::bg::green;
            } else {
                std::cout << rang::bg::gray;
            }
            std::cout << rang::fg::black << to_unicode(b[col][row]);
        }
        std::cout << '\n';
    }
    std::cout << rang::style::reset;
}

inline void pretty_print(const Chess::State& s)
{
    std::cout << (s.active_player == Chess::Player::white ? "White" : "Black") << " to move\n";
    pretty_print(s.board);
}

BOOST_AUTO_TEST_CASE(general_functionality)
{
    const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

    auto state = Chess::State::from_fen(initial_fen);
    //std::cout << "Initial state:\n";
    //pretty_print(state);
    std::cout << "FEN of initial state: " << to_fen(state) << '\n';

    const auto move = calculate_best_move(state);
    std::cout << "Recommended move: " << to_string(move) << '\n';
    state = apply(move, state);

    //std::cout << "New state:\n";
    //pretty_print(state);
    std::cout << "FEN of new state: " << to_fen(state) << '\n';
}

BOOST_AUTO_TEST_CASE(castling_ability)
{
    using namespace Chess;

    const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R";
    auto state = State::from_fen(initial_fen);
    BOOST_CHECK(state.black_castle_rights == Castle_rights::both);
    BOOST_CHECK(state.white_castle_rights == Castle_rights::both);

    auto move = Move{"e1", "g1"};
    state = apply(move, state);
    BOOST_CHECK(to_fen(state) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1RK1");
}

BOOST_AUTO_TEST_CASE(speed)
{
    const auto begin = std::chrono::high_resolution_clock::now();

    const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    auto state = Chess::State::from_fen(initial_fen);
    for (int i = 0; i < 5; ++i) {
        auto move = Chess::calculate_best_move(state);
        state = apply(move, state);
    }

    const auto end = std::chrono::high_resolution_clock::now();

    //pretty_print(state);
    std::cout << "Time taken for 5 half moves: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " ms\n";
}

BOOST_AUTO_TEST_CASE(game)
{
    Chess::Game game;
    const auto state = Chess::State::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    game = Chess::Game{state};
    game.undo();
    game.undo();
    game.undo();
    BOOST_CHECK(to_fen(game.state()) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

BOOST_AUTO_TEST_CASE(legal_move_checking)
{
    auto state = Chess::State::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    state = apply(Chess::Move{"e2", "e4"}, state);
    state = apply(Chess::Move{"a7", "a5"}, state);
    state = apply(Chess::Move{"d1", "h5"}, state);
    state = apply(Chess::Move{"a5", "a4"}, state);
    BOOST_CHECK(state.active_player == Chess::Player::white);
    BOOST_CHECK(Chess::is_legal_move(Chess::Move{"h5", "f7"}, state));

    pretty_print(state);

    state = apply(Chess::Move{"a2", "a3"}, state);
    state = apply(Chess::Move{"b7", "b6"}, state);
    state = apply(Chess::Move{"b2", "b3"}, state);
    state = apply(Chess::Move{"b7", "b6"}, state);
    state = apply(Chess::Move{"c2", "c3"}, state);
    state = apply(Chess::Move{"c8", "a6"}, state);
    BOOST_CHECK(state.active_player == Chess::Player::white);
    BOOST_CHECK(!Chess::is_legal_move(Chess::Move{"e1", "e2"}, state));

    pretty_print(state);
}

