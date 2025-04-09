#include <iostream>
#include <optional>

#include "book.h"
#include "lookup_table.h"

#pragma once

#include <array>
#include <string>
#include <thread>

#include "solver.h"

class OpeningDatabase;

enum class Cell {
    PlayerOne,
    PlayerTwo,
    Empty,
};

inline bool is_empty(Cell c) {
    return c == Cell::Empty;
}

enum class GameState {
    Playing,
    PlayerOneWin,
    PlayerTwoWin,
    Draw,
};

class ArrayBoard {
public:
    std::array<Cell, WIDTH * HEIGHT> cells{};
    std::array<size_t, WIDTH> heights{};
    bool player_one = true;
    std::string game;
    size_t num_moves = 0;
    GameState state = GameState::Playing;

    ArrayBoard() {
        cells.fill(Cell::Empty);
        heights.fill(0);
    }

    static ArrayBoard from_str(const std::string& moves) {
        ArrayBoard board;
        for (char c : moves) {
            if (c < '1' || c > '7') throw std::invalid_argument("Invalid move char");
            board.play_checked(static_cast<size_t>(c - '0'));
        }
        return board;
    }

    bool playable(size_t col) const {
        return col < WIDTH && heights[col] < HEIGHT;
    }

    bool play_checked(size_t column_one_indexed) {
        if (column_one_indexed < 1 || column_one_indexed > WIDTH || !playable(column_one_indexed - 1)) {
            return false;
        }
        size_t col = column_one_indexed - 1;
        if (check_winning_move(col)) {
            state = player_one ? GameState::PlayerOneWin : GameState::PlayerTwoWin;
        } else {
            state = check_draw_move() ? GameState::Draw : GameState::Playing;
        }
        play(col);
        game += std::to_string(column_one_indexed);
        return true;
    }

    void play(size_t col) {
        Cell player = player_one ? Cell::PlayerOne : Cell::PlayerTwo;
        cells[col + WIDTH * heights[col]] = player;
        heights[col]++;
        num_moves++;
        player_one = !player_one;
    }

    bool check_draw_move() const {
        size_t empty_count = 0;
        for (Cell c : cells) if (is_empty(c)) ++empty_count;
        return empty_count == 1;
    }

    bool check_winning_move(size_t col) const {
        Cell player = player_one ? Cell::PlayerOne : Cell::PlayerTwo;
        size_t row = heights[col];

        if (row >= 3) {
            if (
                cells[col + WIDTH * (row - 1)] == player &&
                cells[col + WIDTH * (row - 2)] == player &&
                cells[col + WIDTH * (row - 3)] == player
            ) return true;
        }

        for (int slope = -1; slope <= 1; ++slope) {
            int run = 0;
            for (int dir : {-1, 1}) {
                int x = static_cast<int>(col) + dir;
                int y = static_cast<int>(row) + dir * slope;
                while (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT &&
                       cells[x + WIDTH * y] == player) {
                    ++run;
                    x += dir;
                    y += dir * slope;
                }
            }
            if (run >= 3) return true;
        }
        return false;
    }

    void display() const {
        for (int r = HEIGHT - 1; r >= 0; --r) {
            for (int c = 0; c < WIDTH; ++c) {
                Cell cell = cells[c + WIDTH * r];
                char out = (cell == Cell::PlayerOne) ? 'X' :
                           (cell == Cell::PlayerTwo) ? 'O' : '.';
                std::cout << out;
            }
            std::cout << '\n';
        }
        for (int i = 1; i <= WIDTH; ++i) std::cout << i;
        std::cout << "\n\n";
    }
};

int main() {
    ArrayBoard board;
    LookupTable transposition_table;

    std::cout << "Welcome to Connect 4\n\n";

    std::optional<OpeningDatabase> opening_database;
    try {
        opening_database = OpeningDatabase::load();
    } catch (const std::exception& e) {
        std::cerr << "Opening database not found. Would you like to generate one? (takes a LONG time) [y/n]: ";
        std::string answer;
        std::getline(std::cin, answer);
        if (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y')) {
            // TODO: generate the database here
            std::cout << "Generation logic not implemented. Exiting.\n";
            return 0;
        } else {
            std::cout << "Skipping database generation, expect early AI moves to take ~10 minutes\n";
        }
    }

    bool ai_player1 = false, ai_player2 = false;

    std::string buffer;
    std::cout << "Is player 1 AI controlled? [y/n]: ";
    std::getline(std::cin, buffer);
    if (!buffer.empty() && (buffer[0] == 'y' || buffer[0] == 'Y')) ai_player1 = true;

    std::cout << "Is player 2 AI controlled? [y/n]: ";
    std::getline(std::cin, buffer);
    if (!buffer.empty() && (buffer[0] == 'y' || buffer[0] == 'Y')) ai_player2 = true;

    while (true) {
        board.display();

        if (board.state == GameState::Playing) {
            size_t next_move;

            if ((board.player_one && ai_player1) || (!board.player_one && ai_player2)) {
                std::cout << "AI is thinking...\n";
                //if (ai_player1 && ai_player2) std::this_thread::sleep_for(std::chrono::seconds(3));

                Solver solver(BitBoard::fromMoves(board.game), transposition_table);
                if (opening_database) solver.with_opening_database(*opening_database);

                auto [score, best_move] = solver.solve();
                int win_distance = solver.score_to_win_distance(score);
                std::string move_string = (win_distance == 1) ? "move" : "moves";

                if (score > 0) {
                    int player = board.player_one ? 1 : 2;
                    std::cout << "Player " << player << " can force a win in at most " << win_distance << " " << move_string << ".\n";
                } else if (score < 0) {
                    int player = board.player_one ? 2 : 1;
                    std::cout << "Player " << player << " can force a win in at most " << win_distance << " " << move_string << ".\n";
                } else {
                    int player = board.player_one ? 1 : 2;
                    std::cout << "Player " << player << " can at best force a draw, " << win_distance << " " << move_string << " remaining.\n";
                }

                std::cout << "Best move: " << (best_move + 1) << "\n";
                next_move = best_move + 1;

            } else {
                std::cout << "Move input > ";
                std::string input;
                std::getline(std::cin, input);
                try {
                    next_move = std::stoi(input);
                } catch (...) {
                    std::cerr << "Invalid number: " << input << "\n";
                    continue;
                }
            }

            if (!board.play_checked(next_move)) {
                std::cerr << "Invalid move. Try again.\n";
                continue;
            }

        } else if (board.state == GameState::PlayerOneWin) {
            board.display();
            std::cout << "Player 1 wins!\n";
            break;
        } else if (board.state == GameState::PlayerTwoWin) {
            board.display();
            std::cout << "Player 2 wins!\n";
            break;
        } else if (board.state == GameState::Draw) {
            board.display();
            std::cout << "Draw!\n";
            break;
        }
    }

    return 0;
}
