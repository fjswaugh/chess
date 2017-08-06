#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include "chess/game.h"

BOOST_AUTO_TEST_CASE(general_functionality)
{
    const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    auto game = Game{State::from_fen(initial_fen)};

    const auto move = calculate_best_move(game.state());
    game.state().apply(move);
}

