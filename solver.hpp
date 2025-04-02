#include "bitboard.hpp"
#include "lookupTable.hpp"
#pragma once

class Solver {
    private:
        unsigned long long nodeCount; 
        int columnOrder[WIDTH]; // Order of columns to check
        LookupTable table; // Lookup table for memoization
    public:
        Solver() : nodeCount(0), table(8388593){
            for (int i = 0; i < WIDTH; i++)
            columnOrder[i] = WIDTH/2 + (1-2*(i%2))*(i+1)/2; 
        }

        unsigned long long getNodeCount() const {
            return nodeCount;
        }

        int negamax(const BitBoard& board, int alpha, int beta) {
            nodeCount++;
            if (board.getMoves() == WIDTH * HEIGHT) {
                return 0; // Draw
            }

            for (int i = 0; i < WIDTH; i++) {
                if (board.isColumnEmpty(i) && board.isWin(i)) {
                    return (WIDTH * HEIGHT+1 - board.getMoves())/2; // Win for the current player
                }
            }

            int max = (WIDTH * HEIGHT - 1 - board.getMoves()) / 2; // Upper bound
            if(int score = table.get(board.key())) {
                max = score + MIN_SCORE - 1;
            }
            if (beta > max) {
                beta = max;
                if (alpha >= beta) {
                    return beta; // Beta cut-off
                }
            }

            for (int i = 0; i < WIDTH; ++i) {
                if (board.isColumnEmpty(columnOrder[i])) {
                    BitBoard newBoard(board);
                    newBoard.play(columnOrder[i]);

                    int score = -negamax(newBoard, -beta, -alpha);
                    if (score >= beta) {
                        return score; // Beta cut-off
                    }
                    if (score > alpha) {
                        alpha = score;
                    }
                }
            }
            table.insert(board.key(), alpha - MIN_SCORE + 1); // Store the score in the lookup table
            return alpha; // Return the best score found  
        }
        
    int solve (const BitBoard &board) {
        nodeCount = 0;
        table.reset();
        return negamax(board, -WIDTH*HEIGHT/2, WIDTH*HEIGHT/2);
    }

    unsigned int findBestMove(const BitBoard &board) {
        int bestMove = -1;
        int bestScore = -WIDTH*HEIGHT/2;
        for (int i = 0; i < WIDTH; ++i) {
            int col = columnOrder[i];
            if (board.isColumnEmpty(col)) {
                BitBoard newBoard(board);
                newBoard.play(col);
                if(newBoard.isWin(col)) {
                    return col; // Immediate win
                }
                int score = -negamax(newBoard, -WIDTH*HEIGHT/2, WIDTH*HEIGHT/2);
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = col;
                }
            }
        }
        return bestMove;
    }
};