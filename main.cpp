#include <iostream>
#include <string>
#include "book.h"
#include "lookup_table.h"
#include "solver.h"

std::pair<int, int> runSolver(const std::string& boardSequence, int& moves_to_win) {
    const BitBoard board = BitBoard::fromMoves(boardSequence);
    const LookupTable transposition_table;

    std::optional<OpeningDatabase> opening_database;
    try {
        opening_database = OpeningDatabase::load();
    } catch (const std::exception& e) {
        std::cerr << "Opening database not loaded: " << e.what() << std::endl;
    }

    Solver solver(board, transposition_table);
    if (opening_database) {
        solver.with_opening_database(*opening_database);
    }

    moves_to_win = solver.score_to_win_distance(solver.solve().first);

    return solver.solve();
}

int main() {
    std::string boardSequence;
    if (!std::getline(std::cin, boardSequence)) {
        std::cerr << "Error reading input" << std::endl;
        return 1;
    }
    int move_to_win = -99;
    auto [score, bestMove] = runSolver(boardSequence, move_to_win);

    if (move_to_win > 0) {
        std::cerr << "Position is a win in " << move_to_win << " moves" << std::endl;
    } else if (move_to_win < 0) {
        std::cerr << "Position is a loss in " << -move_to_win << " moves" << std::endl;
    } else {
        std::cerr << "Position is a draw" << std::endl;
    }

    std::cout << "Best move: " << bestMove << std::endl;

    return 0;
}
