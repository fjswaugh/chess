#include "chess/game.h"

int main()
{
    const auto initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    auto game = Game{State::from_fen(initial_fen)};

    Position p = 0;
    //auto moves = generate_moves(game.state().mailbox, Player::white);
    //const auto move = calculate_best_move(game.state());
    //game.state().apply(move);

}

