#include "bitboard.hpp"
#pragma once

static const int columnOrder[7] = {3, 4, 2, 5, 1, 6, 0}; // order of columns to check

class Solver {
    private:
        unsigned long long nodeCount; 
        int maxDepth;
        int startDepth;
    public:
        Solver(int depth = 8) : nodeCount(0), maxDepth(depth), startDepth(0) {} // Constructor to initialize nodeCount to 0

        unsigned long long getNodeCount() const {
            return nodeCount;
        }

        void incrementNodeCount() {
            ++nodeCount;
        }

        void setMaxDepth(int depth) {
            maxDepth = depth;
        }

        int getMaxDepth() const {
            return maxDepth;
        }

        void setStartDepth(int depth) {
            startDepth = depth;
        }

        int getStartDepth() const {
            return startDepth;
        }

        bool canWinNextMove(const BitBoard& board) {
            for (int i = 0; i < 7; ++i) {
                int col = columnOrder[i];
                if (board.isColumnEmpty(col) && board.isWin(col)) {
                    return true;
                }
            }
            return false;
        }

        int negamax (const BitBoard& board, int depth, int alpha, int beta) {
            incrementNodeCount();
            if(isDraw(board) || depth > maxDepth) {
                return 0;
            }

            if (canWinNextMove(board)) {
                return BitBoard::WIDTH * BitBoard::HEIGHT + 1 - board.getMoves()/2;
            }

            int bestValue = BitBoard::WIDTH * BitBoard::HEIGHT - 1 - board.getMoves()/2;
            
            if (beta > bestValue) {
                beta = bestValue;
                if (alpha >= beta) {
                    return beta;
                }
            }

            for (int i = 0; i < 7; ++i) {
                if (board.isColumnEmpty(columnOrder[i])) {
                    BitBoard newBoard = board;
                    newBoard.play(columnOrder[i]);
                    int value = -negamax(newBoard, depth + 1, -beta, -alpha);
                    if (value >= beta) {
                        return value;
                    }
                    if (value > alpha) {
                        alpha = value;
                    }
                }
            }
            return alpha;
        }

        bool isDraw(const BitBoard& board) const { 
            return board.getMoves() == 42;
        }

        int findBestMove(const BitBoard& board) {
            int bestScore = INT32_MIN;
            int bestMove = -1;
            
            for (int i = 0; i < BitBoard::WIDTH; i++) {
                int col = columnOrder[i];
                if (board.isColumnEmpty(col)) {
                    BitBoard newBoard = board;
                    newBoard.play(col);
                    
                    // Check for immediate win
                    if (board.isWin(col)) {
                        return col;
                    }
                    
                    int score = -negamax(newBoard, startDepth, -21, 21);
                    
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = col;
                    }
                }
            }
            
            return bestMove;
        }
};