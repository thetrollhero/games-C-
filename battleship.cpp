#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int BOARD_SIZE = 5; // Size of the board (5x5)
const int SHIP_COUNT = 3; // Number of ships for each player
std::mutex boardMutex;    // Mutex for synchronized access to game output

class Player
{
public:
    std::vector<std::vector<char>> board; // Game board for the player
    int shipsRemaining;                   // Number of remaining ships

    Player() : shipsRemaining(SHIP_COUNT), board(BOARD_SIZE, std::vector<char>(BOARD_SIZE, '~')) {}

    void placeShips()
    {
        for (int i = 0; i < SHIP_COUNT; i++)
        {
            int x, y;
            do
            {
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
            } while (board[x][y] == 'S'); // Ensure no duplicate ship placements
            board[x][y] = 'S';
        }
    }

    bool takeHit(int x, int y)
    {
        if (board[x][y] == 'S')
        {                      // Check if a ship is hit
            board[x][y] = 'X'; // Mark hit
            shipsRemaining--;
            return true;
        }
        else if (board[x][y] == '~')
        {
            board[x][y] = 'O'; // Mark miss
        }
        return false;
    }

    bool allShipsSunk() const
    {
        return shipsRemaining <= 0; // Return true if all ships are sunk
    }

    void displayBoard() const
    {
        for (const auto &row : board)
        {
            for (const auto &cell : row)
            {
                std::cout << cell << ' ';
            }
            std::cout << std::endl;
        }
    }
};

void playerTurn(Player &attacker, Player &defender, int playerId)
{
    while (!defender.allShipsSunk())
    {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;

        {
            std::lock_guard<std::mutex> guard(boardMutex); // Lock for thread safety
            std::cout << "Player " << playerId << " attacks (" << x << ", " << y << ")\n";
            if (defender.takeHit(x, y))
            {
                std::cout << "Hit!\n";
            }
            else
            {
                std::cout << "Miss.\n";
            }
            std::cout << "Player " << playerId << "'s Board:\n";
            attacker.displayBoard();
            std::cout << "Opponent's Board:\n";
            defender.displayBoard();
            std::cout << "--------------------------------\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate delay
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
    Player player1, player2;

    player1.placeShips();
    player2.placeShips();

    std::cout << "Starting Battleship Game!\n";
    std::cout << "Player 1's Board:\n";
    player1.displayBoard();
    std::cout << "Player 2's Board:\n";
    player2.displayBoard();

    std::thread t1(playerTurn, std::ref(player1), std::ref(player2), 1);
    std::thread t2(playerTurn, std::ref(player2), std::ref(player1), 2);

    t1.join();
    t2.join();

    if (player1.allShipsSunk())
    {
        std::cout << "Player 2 Wins!" << std::endl;
    }
    else
    {
        std::cout << "Player 1 Wins!" << std::endl;
    }

    return 0;
}
