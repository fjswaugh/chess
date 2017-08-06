#pragma once

#include <cassert>

#include "player.h"
#include "mailbox.h"

enum class Castle_rights : char {
    both,
    kingside,
    queenside,
    none
};

class Positive_integer {
public:
    Positive_integer(int i) : i_{i} { assert(i > 0); }
    operator int() const { return i_; }
private:
    int i_;
};

struct State {
    static State from_fen(const std::string& fen)
    {
        return State{
            Mailbox::from_fen(fen), Player::white, Castle_rights::both, Castle_rights::both, 1
        };
    }

    void apply(const Move& m) {
        mailbox.apply(m);
        active_player = (active_player == Player::white ? Player::black : Player::white);
        ++turn_number;
    }

    Mailbox mailbox;
    Player active_player = Player::white;
    Castle_rights white_castle_rights = Castle_rights::both;
    Castle_rights black_castle_rights = Castle_rights::both;
    int turn_number = 1;
};

inline std::string to_fen(const State& s)
{
    return to_fen(s.mailbox);
}

Move calculate_best_move(const State&);
int minimax_evaluate(int depth, const State&, bool maximise);

