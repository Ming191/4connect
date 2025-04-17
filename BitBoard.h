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

    /*
     * (board.board_mask + (1ULL << (col * (HEIGHT + 1)))) to find the lowest empty position in column 'col'
     * & column_mask(col) to restrict the move to the column
     */
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

    /*
     *  returns the column index of 'move_bitmap'
     */
    static int columnFromMove(const uint64_t move_bitmap) {
        for (int col = 0; col < WIDTH; ++col) {
            if (move_bitmap & column_mask(col)) return col;
        }
        return WIDTH;
    }
    /*
     * return the mask of the top of the column
     */
    static ull top_mask(const int col) {
        return (1ULL << (HEIGHT-1)) << (col*WIDTH);
    }

    /*
     * return the mask of the bottom of the column
     */
    static ull bottom_mask(const int col) {
        return 1ULL << (col*WIDTH);
    }

    /*
     * return the mask of the column
     */
    static ull column_mask(const int col) {
        return ((1ULL << HEIGHT)-1) << (col*WIDTH);
    }

    /*
     * (top_mask(col) to check whether the top of the column is empty (0) or not (1)
     * if top_mask(col) & board_mask == 0, then the column is playable
     */
    bool playable(int col) const{
        return !(top_mask(col) & board_mask);
    }

    /*
     * pos is the position of the player to simulate state of the game after play(col)
     * (board_mask + bottom_mask(col)) to find the lowest empty position in column 'col'
     * pos =| (board_mask + bottom_mask(col)) & column_mask(col) turn on the bit at the empty position
     */
    bool checkWinningMove(int col) const {
        uint64_t pos = player_mask;
        pos |= (board_mask + bottom_mask(col)) & column_mask(col);

        // horizontal
        uint64_t m = pos & (pos >> (HEIGHT + 1));
        if (m & (m >> (2 * (HEIGHT + 1)))) return true;

        // diagonal '\'
        m = pos & (pos >> HEIGHT);
        if (m & (m >> (2 * HEIGHT))) return true;

        // diagonal '/'
        m = pos & (pos >> (HEIGHT + 2));
        if (m & (m >> (2 * (HEIGHT + 2)))) return true;

        // vertical
        m = pos & (pos >> 1);
        if (m & (m >> 2)) return true;

        return false;
    }

    void play(const uint64_t move_bitmap) {
        player_mask ^= board_mask;
        board_mask |= move_bitmap;
        moves++;
    }

    int moveScore(const ull candidate) const {
        return __builtin_popcountll(winningPositions(player_mask | candidate));
    }


    uint32_t huffmanCode() const {
        return std::min(huffmanCodeImpl(false), huffmanCodeImpl(true));
    }

    uint64_t winningPositions(const uint64_t mask) const {
        // vertical
        // top-end
        uint64_t r = (mask << 1) & (mask << 2) & (mask << 3);

        //horizontal
        uint64_t p = (mask << (HEIGHT + 1)) & (mask << (2 * (HEIGHT + 1)));
        // right end
        r |= p & (mask << (3 * (HEIGHT + 1)));
        //O O _ O
        r |= p & (mask >> (HEIGHT + 1));
        p = (mask >> (HEIGHT + 1)) & (mask >> (2 * (HEIGHT + 1)));

        //left end
        r |= p & (mask >> (3 * (HEIGHT + 1)));
        //O _ O O
        r |= p & (mask << (HEIGHT + 1));

        //diagonal /
        p = (mask << HEIGHT) & (mask << (2 * HEIGHT));

        //right end
        r |= p & (mask << (3 * HEIGHT));
        //O O _ O
        r |= p & (mask >> HEIGHT);

        p = (mask >> HEIGHT) & (mask >> (2 * HEIGHT));
        //left end
        r |= p & (mask >> (3 * HEIGHT));

        //O _ O O
        r |= p & (mask << HEIGHT);

        // diagonal '\'
        p = (mask << (HEIGHT + 2)) & (mask << (2 * (HEIGHT + 2)));

        //right-end
        r |= p & (mask << (3 * (HEIGHT + 2)));
        //O O _ O
        r |= p & (mask >> (HEIGHT + 2));
        p = (mask >> (HEIGHT + 2)) & (mask >> (2 * (HEIGHT + 2)));

        //left-end
        r |= p & (mask >> (3 * (HEIGHT + 2)));
        //O _ O O
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
        const uint64_t opp_wins = opponentWinningPositions();
        const uint64_t forced = moves & opp_wins;
        if (forced != 0) {
            if ((forced & (forced - 1)) != 0) return 0;
            moves = forced;
        }
        return moves & ~(opp_wins >> 1);

    }

    ull getHash() const {
        return player_mask + board_mask;
    }

private:
    ull player_mask;
    ull board_mask;
    unsigned int moves;

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
            const uint64_t col_mask = column_mask(col);
            for (int row = 0; row <= HEIGHT; ++row) {
                const uint64_t tile_mask = col_mask & (staticBottomMask() << row);
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
