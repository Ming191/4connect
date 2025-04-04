//
// Created by min on 4/4/2025.
//

#ifndef BITBOARD_H
#define BITBOARD_H
#include <cstdint>
#include <string>
#include <iostream>

// * .  .  .  .  .  .  .
// * 5 12 19 26 33 40 47
// * 4 11 18 25 32 39 46
// * 3 10 17 24 31 38 45
// * 2  9 16 23 30 37 44
// * 1  8 15 22 29 36 43
// * 0  7 14 21 28 35 42

using ull = uint64_t;

static constexpr int WIDTH = 7;
static constexpr int HEIGHT = 6;
static constexpr int MIN_SCORE = -(WIDTH * HEIGHT) / 2 + 3;
static constexpr int MAX_SCORE = (WIDTH * HEIGHT) / 2 - 3;

class Bitboard {
public:
    Bitboard(): board{0}, mask{0}, moves{0} {};

    int getMoves() const {
        return static_cast<int>(moves);
    }

    bool canPlay(int col) const{
        return !(top_mask(col) & mask);
    }

    void play(int col) {
        board ^= mask;
        mask |= mask + bottom_mask(col);
        moves++;
    }

    bool canWinNextMove(int col) const {
        ull pos(board);
        pos |= (mask + bottom_mask(col)) & column_mask((col));
        return check(pos);
    }

    unsigned int play(const std::string &seq) {
        for(unsigned int i = 0; i < seq.size(); i++) {
            int col = seq[i] - '1';
            if(col < 0 || col >= WIDTH || !canPlay(col) || canWinNextMove(col)) return i;
            play(col);
        }
        return seq.size();
    }

    void show() const {
        for (int row = HEIGHT - 1; row >= 0; row--) {
            for (int col = 0; col < WIDTH; col++) {
                ull pos = 1ULL << (row + col * WIDTH);
                if (board & pos) {
                    std::cout << "X ";  // Player 1
                } else if (mask & pos) {
                    std::cout << "O ";  // Player 2
                } else {
                    std::cout << ". ";  // Empty space
                }
            }
            std::cout << "\n";
        }
        std::cout << "---------------\n";
        std::cout << "1 2 3 4 5 6 7 \n"; // Column numbers
    }

private:
    ull board;
    ull mask;
    unsigned int moves;

    static ull top_mask(const int col) {
        return (1ULL << (HEIGHT-1)) << (col*WIDTH);
    };
    static ull bottom_mask(const int col) {
        return 1ULL << (col*WIDTH);
    }
    static ull column_mask(const int col) {
        return ((1ULL << HEIGHT)-1) << (col*WIDTH);
    }

    static bool check(const ull pos) {
        if(const ull tmp = pos & (pos >> WIDTH); tmp & (tmp >> (2*WIDTH))) return true;
        if(const ull tmp = pos & (pos >> 1); tmp & (tmp >> 2)) return true;
        if(const ull tmp = pos & (pos >> 8); tmp & (tmp >> 16)) return true;
        if(const ull tmp = pos & (pos >> 6); tmp & (tmp >> 12)) return true;
        return false;
    }
};

#endif //BITBOARD_H
