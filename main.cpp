#include "bitboard.h"
#include "solver.h"
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

int main() {
    Bitboard board;
    Solver solver;
    std::string line;

    std::ifstream file(R"(C:\Users\min\CLionProjects\4connect\test.txt)");
    std::ofstream report(R"(C:\Users\min\CLionProjects\4connect\report_version_1_0_1.txt)");

    if (!file) {
        std::cerr << "Error: Unable to open 'test.txt'.\n";
        return 1;
    }

    if (!report) {
        std::cerr << "Error: Unable to create 'report.txt'.\n";
        return 1;
    }

    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;
        std::cout << "Processing Line " << lineCount << ": " << line << "\n";
        report << "Processing Line " << lineCount << ": " << line << "\n";

        Bitboard board; // Reset board for each game sequence
        if (board.play(line) != line.size()) {
            std::cout << "Invalid move sequence on Line " << lineCount << "\n";
            report << "Invalid move sequence on Line " << lineCount << "\n";
            continue;
        }

        // Redirect board state to report
        std::ostringstream boardStream;
        std::streambuf* oldCoutBuffer = std::cout.rdbuf(boardStream.rdbuf());
        board.show();
        std::cout.rdbuf(oldCoutBuffer);  // Restore original std::cout buffer

        std::cout << boardStream.str();  // Print to console
        report << boardStream.str();     // Write board state to file

        // Start timer
        auto start = std::chrono::high_resolution_clock::now();

        int score = solver.solve(board);

        // End timer and calculate duration
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Score: " << score << "\n";
        std::cout << "Nodes visited: " << solver.getNodeVisited() << "\n";
        std::cout << "Time taken: " << duration.count() << " ms\n\n";

        report << "Score: " << score << "\n";
        report << "Nodes visited: " << solver.getNodeVisited() << "\n";
        report << "Time taken: " << duration.count() << " ms\n\n";
    }

    std::cout << "Report saved to 'report.txt'\n";
    report.close();
}
