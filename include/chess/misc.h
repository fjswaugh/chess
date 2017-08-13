#pragma once

namespace Chess {

enum class Player : bool {
    white, black
};

inline Player opponent_of(Player p) {
    return Player(!static_cast<bool>(p));
}

enum class Castle_rights : char {
    both,
    kingside,
    queenside,
    none
};

}  // namespace Chess

