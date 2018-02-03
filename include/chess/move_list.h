#pragma once

#include "chess/move.h"

namespace Chess {

/**
 * Move list is rather like a std::vector<Move>, but without heap allocation.
 * The size of the list is limited to 256, and behaviour is currently undefined
 * should that size be exceeded. Fortunately, there is no chess position that
 * generates more than 256 possible moves.
 */
struct Move_list {
    Move_list()
        : size_{0}
    {}

    Move operator[](int i) const {
        return data_[i];
    }
    Move& operator[](int i) {
        return data_[i];
    }

    Move* begin() {
        return data_;
    }
    Move* end() {
        return data_ + size_;
    }
    const Move* begin() const {
        return data_;
    }
    const Move* end() const {
        return data_ + size_;
    }

    const Move& front() const {
        assert(this->size() > 0);
        return data_[0];
    }
    const Move& back() const {
        assert(this->size() > 0);
        return data_[size_ - 1];
    }
    Move& front() {
        assert(this->size() > 0);
        return data_[0];
    }
    Move& back() {
        assert(this->size() > 0);
        return data_[size_ - 1];
    }

    void push_back(Move m) {
        data_[size_] = m;
        ++size_;
    }

    template <typename... T>
    void emplace_back(T&&... t) {
        data_[size_] = Move(std::forward<T>(t)...);
        ++size_;
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }
private:
    Move data_[256];

    std::size_t size_;
};

}

