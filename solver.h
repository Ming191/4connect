// Solver for Connect 4 - rewritten in C++ to mirror Pascal Pons' Rust implementation

#ifndef SOLVER_H
#define SOLVER_H

#include "BitBoard.h"
#include "book.h"
#include "lookup_table.h"
#include <array>
#include <optional>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>


class Solver {
public:
    Solver(const BitBoard &board)
        : board(board), node_count(0), transposition_table(), opening_database(std::nullopt) {}

    Solver(const BitBoard &board, LookupTable tt)
        : board(board), node_count(0), transposition_table(std::move(tt)), opening_database(std::nullopt) {}

    Solver& with_opening_database(const OpeningDatabase& db) {
        opening_database = db;
        return *this;
    }

    std::pair<int, int> solve() {
        return _solve(true);
    }

    std::pair<int, int> solve_verbose() {
        return _solve(false);
    }

    int score_to_win_distance(int score) const {
        if (score == 0) {
            return WIDTH * HEIGHT - board.getMoves();
        } else if (score > 0) {
            return (WIDTH * HEIGHT / 2 + 1 - score) - board.getMoves() / 2;
        } else {
            return (WIDTH * HEIGHT / 2 + 1 + score) - board.getMoves() / 2;
        }
    }

private:
    BitBoard board;
    size_t node_count;
    LookupTable transposition_table;
    std::optional<OpeningDatabase> opening_database;

    struct MoveSorter {
        size_t size = 0;
        std::array<std::tuple<uint64_t, int, int>, WIDTH> moves;

        void push(uint64_t move, int column, int score) {
            size_t pos = size++;
            while (pos > 0 && std::get<2>(moves[pos - 1]) > score) {
                moves[pos] = moves[pos - 1];
                --pos;
            }
            moves[pos] = {move, column, score};
        }

        bool has_next() const {
            return size > 0;
        }

        std::pair<uint64_t, int> next() {
            assert(size > 0);
            return {std::get<0>(moves[--size]), std::get<1>(moves[size])};
        }
    };

    static constexpr std::array<int, WIDTH> move_order() {
        std::array<int, WIDTH> order{};
        for (int i = 0; i < WIDTH; ++i) {
            order[i] = WIDTH / 2 + (i % 2 ? (i + 1) / 2 : -(i / 2));
        }
        return order;
    }

    int negamax(int alpha, int beta) {
        ++node_count;

        for (int col = 0; col < WIDTH; ++col) {
            if (board.playable(col) && board.checkWinningMove(col)) {
                return (WIDTH * HEIGHT + 1 - board.getMoves()) / 2;
            }
        }

        uint64_t non_losing = board.nonLosingMoves();
        if (non_losing == 0) {
            return -(WIDTH * HEIGHT - board.getMoves()) / 2;
        }

        if (board.getMoves() == WIDTH * HEIGHT) return 0;

        if (board.getMoves() == DATABASE_DEPTH && opening_database.has_value()) {
            auto maybe_score = opening_database->get(board.huffmanCode());
            if (maybe_score.has_value()) return *maybe_score;
        }

        int max = (WIDTH * HEIGHT - 1 - board.getMoves()) / 2;

        uint64_t key = board.getHash();
        int val = transposition_table.get(key);
        if (val != 0) {
            if (val > MAX_SCORE - MIN_SCORE + 1) {
                int min = val + 2 * MIN_SCORE - MAX_SCORE - 2;
                alpha = std::max(alpha, min);
                if (alpha >= beta) return alpha;
            } else {
                max = val + MIN_SCORE - 1;
                beta = std::min(beta, max);
                if (alpha >= beta) return beta;
            }
        }

        beta = std::min(beta, max);
        if (alpha >= beta) return beta;

        MoveSorter sorter;
        auto order = move_order();
        for (int i = WIDTH - 1; i >= 0; --i) {
            int col = order[i];
            uint64_t candidate = non_losing & BitBoard::column_mask(col);
            if (candidate && board.playable(col)) {
                sorter.push(candidate, col, board.moveScore(candidate));
            }
        }

        while (sorter.has_next()) {
            auto [move, col] = sorter.next();
            Solver next = *this;
            next.node_count = 0;
            next.board.play(move);
            int score = -next.negamax(-beta, -alpha);
            node_count += next.node_count;

            if (score >= beta) {
                transposition_table.set(key, (score + MAX_SCORE - 2 * MIN_SCORE + 2));
                return score;
            }
            alpha = std::max(alpha, score);
        }

        transposition_table.set(key, (alpha - MIN_SCORE + 1));
        return alpha;
    }

    std::pair<int, int> top_level_search(int alpha, int beta) {
        ++node_count;

        for (int col = 0; col < WIDTH; ++col) {
            if (board.playable(col) && board.checkWinningMove(col)) {
                return {(WIDTH * HEIGHT + 1 - board.getMoves()) / 2, col};
            }
        }

        uint64_t non_losing = board.nonLosingMoves();
        if (non_losing == 0) {
            for (int i = 0; i < WIDTH; ++i) {
                if (board.playable(i)) {
                    return {-(WIDTH * HEIGHT - board.getMoves()) / 2, i};
                }
            }
        }

        if (board.getMoves() == WIDTH * HEIGHT) return {0, WIDTH};

        MoveSorter sorter;
        auto order = move_order();
        for (int i = WIDTH - 1; i >= 0; --i) {
            int col = order[i];
            uint64_t candidate = non_losing & BitBoard::column_mask(col);
            if (candidate && board.playable(col)) {
                sorter.push(candidate, col, board.moveScore(candidate));
            }
        }

        int best_score = MIN_SCORE;
        int best_move = WIDTH;

        while (sorter.has_next()) {
            auto [move, col] = sorter.next();
            Solver next = *this;
            next.node_count = 0;
            next.board.play(move);
            int score = -next.negamax(-beta, -alpha);
            node_count += next.node_count;

            if (score >= beta) return {score, col};
            if (score > best_score) {
                best_score = score;
                best_move = col;
            }
            alpha = std::max(alpha, score);
        }

        return {alpha, best_move};
    }

    std::pair<int, int> _solve(bool silent) {
        int min = -(WIDTH * HEIGHT - board.getMoves()) / 2;
        int max = (WIDTH * HEIGHT + 1 - board.getMoves()) / 2;
        int best_move = WIDTH;

        while (min < max) {
            int mid = min + (max - min) / 2;
            if (mid <= 0 && min / 2 < mid) mid = min / 2;
            else if (mid >= 0 && max / 2 > mid) mid = max / 2;

            if (!silent) {
                std::cout << "Search depth: " << (WIDTH * HEIGHT - board.getMoves() - std::min(abs(min), abs(max)))
                          << "/" << (WIDTH * HEIGHT - board.getMoves())
                          << ", uncertainty: " << (max - min) << std::endl;
            }

            auto [result, move] = top_level_search(mid, mid + 1);
            best_move = move;

            if (result <= mid) max = result;
            else min = result;
        }
        return {min, best_move};
    }
};

#endif //SOLVER_H