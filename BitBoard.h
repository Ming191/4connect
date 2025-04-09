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

class BitBoard {
public:
    BitBoard(): player_mask{0}, board_mask{0}, moves{0} {};

    int getMoves() const {
        return static_cast<int>(moves);
    }

    uint64_t getPlayerMask() const {
        return player_mask;
    }

    uint64_t getBoardMask() const {
        return board_mask;
    }

    static BitBoard fromMoves(const std::string& moves) {
        BitBoard board;
        for (char c : moves) {
            int col = c - '0';
            if (col < 1 || col > WIDTH) throw std::invalid_argument("Invalid move character");
            col -= 1;
            if (!board.playable(col)) throw std::invalid_argument("Column full");
            if (board.checkWinningMove(col)) throw std::invalid_argument("Game already won");
            const uint64_t move = (board.board_mask + (1ULL << (col * (HEIGHT + 1)))) & column_mask(col);
            board.play(move);
        }
        return board;
    }

    static int columnFromMove(const uint64_t move_bitmap) {
        for (int col = 0; col < WIDTH; ++col) {
            if (move_bitmap & column_mask(col)) return col;
        }
        return WIDTH;
    }

    static ull top_mask(const int col) {
        return (1ULL << (HEIGHT-1)) << (col*WIDTH);
    }
    static ull bottom_mask(const int col) {
        return 1ULL << (col*WIDTH);
    }
    static ull column_mask(const int col) {
        return ((1ULL << HEIGHT)-1) << (col*WIDTH);
    }

    bool playable(int col) const{
        return !(top_mask(col) & board_mask);
    }

    bool checkWinningMove(int col) const {
        uint64_t pos = player_mask;
        pos |= (board_mask + bottom_mask(col)) & column_mask(col);

        uint64_t m;

        m = pos & (pos >> (HEIGHT + 1));
        if (m & (m >> (2 * (HEIGHT + 1)))) return true;

        m = pos & (pos >> HEIGHT);
        if (m & (m >> (2 * HEIGHT))) return true;

        m = pos & (pos >> (HEIGHT + 2));
        if (m & (m >> (2 * (HEIGHT + 2)))) return true;

        m = pos & (pos >> 1);
        if (m & (m >> 2)) return true;

        return false;
    }

    void play(const uint64_t move_bitmap) {
        player_mask ^= board_mask;
        board_mask |= move_bitmap;
        moves++;
    }

    bool canWinNextMove(int col) const {
        ull pos(player_mask);
        pos |= (board_mask + bottom_mask(col)) & column_mask((col));
        return check(pos);
    }

    int moveScore(const ull candidate) const {
        return __builtin_popcountll(winningPositions(player_mask | candidate));
    }


    uint32_t huffmanCode() const {
        return std::min(huffmanCodeImpl(false), huffmanCodeImpl(true));
    }

    uint64_t winningPositions(const uint64_t mask) const {
        uint64_t r = (mask << 1) & (mask << 2) & (mask << 3);
        uint64_t p = (mask << (HEIGHT + 1)) & (mask << (2 * (HEIGHT + 1)));
        r |= p & (mask << (3 * (HEIGHT + 1)));
        r |= p & (mask >> (HEIGHT + 1));
        p = (mask >> (HEIGHT + 1)) & (mask >> (2 * (HEIGHT + 1)));
        r |= p & (mask >> (3 * (HEIGHT + 1)));
        r |= p & (mask << (HEIGHT + 1));

        p = (mask << HEIGHT) & (mask << (2 * HEIGHT));
        r |= p & (mask << (3 * HEIGHT));
        r |= p & (mask >> HEIGHT);
        p = (mask >> HEIGHT) & (mask >> (2 * HEIGHT));
        r |= p & (mask >> (3 * HEIGHT));
        r |= p & (mask << HEIGHT);

        p = (mask << (HEIGHT + 2)) & (mask << (2 * (HEIGHT + 2)));
        r |= p & (mask << (3 * (HEIGHT + 2)));
        r |= p & (mask >> (HEIGHT + 2));
        p = (mask >> (HEIGHT + 2)) & (mask >> (2 * (HEIGHT + 2)));
        r |= p & (mask >> (3 * (HEIGHT + 2)));
        r |= p & (mask << (HEIGHT + 2));

        return r & (staticFullBoardMask() ^ board_mask);
    }

    uint64_t possibleMoves() const {
        return (board_mask + staticBottomMask()) & staticFullBoardMask();
    }

    uint64_t opponentWinningPositions() const {
        uint64_t opp_mask = player_mask ^ board_mask;
        return winningPositions(opp_mask);
    }

    uint64_t nonLosingMoves() const {
        uint64_t moves = possibleMoves();
        uint64_t opp_wins = opponentWinningPositions();
        uint64_t forced = moves & opp_wins;
        if (forced != 0) {
            if ((forced & (forced - 1)) != 0) return 0;
            moves = forced;
        }
        return moves & ~(opp_wins >> 1);

    }

    unsigned int play(const std::string &seq) {
        for(unsigned int i = 0; i < seq.size(); i++) {
            int col = seq[i] - '1';
            if(col < 0 || col >= WIDTH || !playable(col) || canWinNextMove(col)) return i;
            play(col);
        }
        return seq.size();
    }

    void show() const {
        for (int row = HEIGHT - 1; row >= 0; row--) {
            for (int col = 0; col < WIDTH; col++) {
                ull pos = 1ULL << (row + col * WIDTH);
                if (player_mask & pos) {
                    std::cout << "X ";  // Player 1
                } else if (board_mask & pos) {
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

    ull getHash() const {
        return player_mask + board_mask;
    }

private:
    ull player_mask;
    ull board_mask;
    unsigned int moves;

    static bool check(const ull pos) {
        if(const ull tmp = pos & (pos >> WIDTH); tmp & (tmp >> (2*WIDTH))) return true;
        if(const ull tmp = pos & (pos >> 1); tmp & (tmp >> 2)) return true;
        if(const ull tmp = pos & (pos >> 8); tmp & (tmp >> 16)) return true;
        if(const ull tmp = pos & (pos >> 6); tmp & (tmp >> 12)) return true;
        return false;
    }

    static uint64_t staticBottomMask() {
        uint64_t mask = 0;
        for (int col = 0; col < WIDTH; ++col) {
            mask |= 1ULL << (col * (HEIGHT + 1));
        }
        return mask;
    }

    static uint64_t staticFullBoardMask() {
        return staticBottomMask() * ((1ULL << HEIGHT) - 1);
    }

    uint32_t huffmanCodeImpl(const bool mirror) const {
        uint32_t code = 0;
        for (int i = 0; i < WIDTH; ++i) {
            int col = mirror ? (WIDTH - 1 - i) : i;
            uint64_t col_mask = column_mask(col);
            for (int row = 0; row <= HEIGHT; ++row) {
                uint64_t tile_mask = col_mask & (staticBottomMask() << row);
                if ((board_mask & tile_mask) == 0) {
                    code <<= 1;
                    break;
                }
                if (player_mask & tile_mask) {
                    code = (code << 2) + 0b10;
                } else {
                    code = (code << 2) + 0b11;
                }
            }
        }
        return code << 1;
    }

};

#endif //BITBOARD_H
