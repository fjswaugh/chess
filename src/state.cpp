#include "chess/state.h"

namespace Chess {

State State::from_fen(const std::string& fen_str)
{
    return State{Board::from_fen(fen_str)};
}

std::string to_fen(const State& s)
{
    return to_fen(s.board);
}

Board Board::from_fen(const std::string& fen_str) {
    // Just use the first part of the fen_str, ignoring anything extra

    Board b;

    int row = 7;
    int col = 0;

    for (char ch : fen_str) {
        if (ch == '/') continue;

        if (ch > '0' && ch <= '9') {
            col += (ch - '0');
        } else {
            b[col][row] = to_square(ch);
            ++col;
        }

        if (col > 7) {
            col = 0;
            --row;
        }
        if (row < 0) break;
    }

    return b;
}

std::string to_fen(const Board& b)
{
    std::string result;
    int count = 0;

    auto push_back_count = [&result, &count] {
        if (count > 0) {
            result.push_back(count + '0');
            count = 0;
        }
    };

    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            if (is_empty(b[col][row])) {
                ++count;
            } else {
                push_back_count();
                result.push_back(to_char(b[col][row]));
            }
        }
        push_back_count();
        result.push_back('/');
    }
    result.pop_back();  // Pop back the last slash

    return result;
}

}  // namespace Chess

