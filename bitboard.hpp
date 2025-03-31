#include <cstdint>

/*
	 * .  .  .  .  .  .  .
	 * 5 12 19 26 33 40 47
	 * 4 11 18 25 32 39 46
	 * 3 10 17 24 31 38 45
	 * 2  9 16 23 30 37 44
	 * 1  8 15 22 29 36 43
	 * 0  7 14 21 28 35 42 
*/

class BitBoard 
{
    private:
        uint64_t current_player;
        uint64_t mask;
        unsigned int moves;

        static bool checkRow(uint64_t pos) 
        {
            uint64_t next = pos & (pos >> 7);
            return (next & (next >> 14)) != 0;
        }  

        static bool checkCol(uint64_t pos) 
        {
            uint64_t next = pos & (pos >> 1);
            return (next & (next >> 2)) != 0;
        }

        static bool checkDiagonalUp(uint64_t pos) 
        {
            uint64_t next = pos & (pos >> 8);
            return (next & (next >> 16)) != 0;
        }

        static bool checkDiagonalDown(uint64_t pos) 
        {
            uint64_t next = pos & (pos >> 6);
            return (next & (next >> 12)) != 0;
        }

        static uint64_t topMask(int col) 
        {
            return (1ULL << 5) << (col * 7); // turn top bit of col 'para' to 1
        }

        static uint64_t bottomMask(int col) 
        {
            return (1ULL << 0) << (col * 7); // turn bottom bit of col 'para' to 1
        }

        static uint64_t columnMask(int col) 
        {
            return ((1ULL << 6) - 1) << (col * 7); // turn all bits in a column to 1
        }

    public:
        static constexpr int WIDTH = 7;
        static constexpr int HEIGHT = 6;

        BitBoard(): current_player(0), mask(0), moves(0) {}

        bool isColumnEmpty(int col) const
        {
            return (mask & topMask(col)) == 0;
        }

        void play (int col) 
        {
            if (isColumnEmpty(col)) 
            {
                current_player ^= mask;
                mask |= mask + bottomMask(col);
                moves++;
            }
        }

        static bool checkGeneric(uint64_t pos) 
        {
            return checkRow(pos) || checkCol(pos) || checkDiagonalUp(pos) || checkDiagonalDown(pos);
        }

        bool isWin(int col) const 
        {
            uint64_t player = current_player;
            player |= (mask + bottomMask(col)) & columnMask(col); // add the last move to the player
            return checkGeneric(player);
        }

        unsigned int getMoves() const 
        {
            return moves;
        }

        uint64_t key() const 
        {
            return current_player + mask;
        }
};