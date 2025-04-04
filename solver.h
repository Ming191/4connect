//
// Created by min on 4/4/2025.
//

#ifndef SOLVER_H
#define SOLVER_H

#include "bitboard.h"
#include <array>

class Solver {
public:
    Solver() : nodeVisited(0) {
        for (int i = 0; i < WIDTH; i++) {
            columnOrder[i] = (WIDTH / 2) + ((i + 1) / 2) * (1 - 2 * (i & 1));
        }
    }

    int solve(const Bitboard& board) {
        reset();
        return negamax(board, -21, 21);
    }

    void reset() {
        nodeVisited = 0;
    }

    unsigned int getNodeVisited() const {
        return nodeVisited;
    }
private:
    unsigned int nodeVisited;
    std::array<int, 7> columnOrder{};

    int negamax(const Bitboard &board, int alpha, int beta) {
        nodeVisited++;

        if(board.getMoves() == WIDTH * HEIGHT) return 0;

        // Immediate win check
        for (int x = 0; x < WIDTH; x++) {
            if(board.canPlay(x) && board.canWinNextMove(x)) {
                return (WIDTH * HEIGHT + 1 - board.getMoves()) / 2;
            }
        }

        if (int max = (WIDTH * HEIGHT - 1 - board.getMoves()) / 2; beta > max) {
            beta = max;
            if(alpha >= beta) return beta;
        }

        for (int i : columnOrder) {
            if(board.canPlay(i)) {
                Bitboard newBoard(board);
                newBoard.play(i);
                int score = -negamax(newBoard, -beta, -alpha);
                if(score >= beta) return beta;
                if(score > alpha) alpha = score;
            }
        }
        return alpha;
    }
};

#endif // SOLVER_H
