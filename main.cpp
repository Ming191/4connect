#include <iostream>
#include "solver.hpp"
#include "bitboard.hpp"

// Class to handle game display and logic
class ConnectFourGame {
private:
    BitBoard board;
    Solver solver;
    char grid[BitBoard::HEIGHT][BitBoard::WIDTH];
    bool humanTurn;
    
    // Returns the lowest empty row in the given column
    int getEmptyRow(int col) const {
        for (int row = 0; row < BitBoard::HEIGHT; row++) {
            if (grid[row][col] == '.') {
                return row;
            }
        }
        return -1; // Column is full
    }
    
public:
    ConnectFourGame(bool humanGoesFirst = true, int aiDepth = 8) : solver(aiDepth), humanTurn(humanGoesFirst) {
        // Initialize grid
        for (int row = 0; row < BitBoard::HEIGHT; row++) {
            for (int col = 0; col < BitBoard::WIDTH; col++) {
                grid[row][col] = '.';
            }
        }
    }
    
    void setAIDepth(int depth) {
        solver.setMaxDepth(depth);
    }
    
    int getAIDepth() const {
        return solver.getMaxDepth();
    }
    
    void printBoard() const {
        std::cout << "\n";
        for (int row = BitBoard::HEIGHT - 1; row >= 0; row--) {
            std::cout << "| ";
            for (int col = 0; col < BitBoard::WIDTH; col++) {
                std::cout << grid[row][col] << " ";
            }
            std::cout << "|\n";
        }
        std::cout << "| ";
        for (int col = 0; col < BitBoard::WIDTH; col++) {
            std::cout << col << " ";
        }
        std::cout << "|\n\n";
    }
    
    bool makeMove(int col) {
        if (!board.isColumnEmpty(col)) {
            return false;
        }
        
        int row = getEmptyRow(col);
        grid[row][col] = humanTurn ? 'X' : 'O';
        
        bool isWinningMove = board.isWin(col);
        board.play(col);
        
        return isWinningMove;
    }
    
    int getAIMove() {
        return solver.findBestMove(board);
    }
    
    bool isColumnValid(int col) const {
        return col >= 0 && col < BitBoard::WIDTH && board.isColumnEmpty(col);
    }
    
    bool isDraw() const {
        return solver.isDraw(board);
    }
    
    void switchTurn() {
        humanTurn = !humanTurn;
    }
    
    bool isHumanTurn() const {
        return humanTurn;
    }
    
    unsigned long long getNodeCount() const {
        return solver.getNodeCount();
    }
};

int main() {
    std::cout << "Welcome to Connect Four!\n";
    std::cout << "You are X, the AI is O.\n\n";
    
    // Ask for AI difficulty (depth)
    std::cout << "Choose AI depth:\n";
    std::cout << "Enter your choice: ";
    
    int depthChoice;
    int aiDepth = 8; // Default
    
    bool validDepthChoice = false;
    while (!validDepthChoice) {
        std::cin >> depthChoice;
        
        if (std::cin.fail() || depthChoice < 1 || depthChoice > 41) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid choice. Please enter a number between 1 and 41: ";
            continue;
        }
        
        validDepthChoice = true;
    }

    aiDepth = depthChoice;

    // Ask if player wants to go first or second
    std::cout << "Do you want to go first or second?\n";
    std::cout << "1: First (You start)\n";
    std::cout << "2: Second (AI starts)\n";
    std::cout << "Enter your choice (1 or 2): ";
    
    int choice;
    bool validChoice = false;
    bool humanGoesFirst = true;
    
    while (!validChoice) {
        std::cin >> choice;
        
        if (std::cin.fail() || (choice != 1 && choice != 2)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid choice. Please enter 1 or 2: ";
            continue;
        }
        
        validChoice = true;
    }
    
    humanGoesFirst = (choice == 1);
    
    ConnectFourGame game(humanGoesFirst, aiDepth);
    std::cout << "\nAI difficulty set to depth: " << game.getAIDepth() << std::endl;
    
    bool gameOver = false;
    
    std::cout << "\nEnter a column number (0-6) to drop your piece.\n\n";
    
    game.printBoard();
    
    while (!gameOver) {
        if (game.isHumanTurn()) {
            // Human's turn
            int col;
            bool validMove = false;
            
            while (!validMove) {
                std::cout << "Your move (0-6): ";
                std::cin >> col;
                
                if (std::cin.fail() || !game.isColumnValid(col)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid move. Please enter a valid column number.\n";
                    continue;
                }
                
                validMove = true;
            }
            
            // Make the move and check if player won
            bool won = game.makeMove(col);
            game.printBoard();
            
            if (won) {
                std::cout << "Congratulations! You win!\n";
                gameOver = true;
                break;
            }
        } else {
            // AI's turn
            std::cout << "AI is thinking...\n";
            
            int col = game.getAIMove();
            std::cout << "AI plays column " << col << ".\n";
            
            // Make the move and check if AI won
            bool won = game.makeMove(col);
            game.printBoard();
            
            if (won) {
                std::cout << "AI wins! Better luck next time.\n";
                gameOver = true;
                break;
            }
        }
        
        // Check for a draw
        if (game.isDraw()) {
            std::cout << "It's a draw!\n";
            gameOver = true;
            break;
        }
        
        // Switch turns
        game.switchTurn();
    }
    
    std::cout << "Game over. Total nodes explored by AI: " << game.getNodeCount() << "\n";
    
    return 0;
}