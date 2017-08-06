#pragma once

enum class Player : bool {
    white, black
};

inline Player opponent_of(Player p) {
    return Player(!static_cast<bool>(p));
}

