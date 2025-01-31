#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

const int BOARD_SIZE = 8;

enum Piece
{
    EMPTY,
    USER,
    BOT,
    USER_KING,
    BOT_KING
};

struct Move
{
    int fromRow, fromCol;
    int toRow, toCol;
    bool isCapture;
    int capturedRow, capturedCol;
};

void printBoard(const std::vector<std::vector<Piece> > &board)
{
    std::cout << "  ";
    for (int col = 0; col < BOARD_SIZE; ++col)
    {
        std::cout << col << " ";
    }
    std::cout << "\n";

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        std::cout < < row < < " ";
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if (board[row][col] == USER)
                std::cout < < "U ";
            else if (board[row][col] == BOT)
                std::cout < < "B ";
            else if (board[row][col] == USER_KING)
                std::cout << "K ";
            else if (board[row][col] == BOT_KING)
                std::cout < < "Q ";
            else
                std::cout < < ". ";
        }
        std::cout << "\n";
    }
}

bool isValidCapture(const std::vector<std::vector<Piece>> &board, int fromRow, int fromCol, int toRow, int toCol, Piece player)
{
    if (toRow < 0 || toRow >= BOARD_SIZE || toCol < 0 || toCol >= BOARD_SIZE || board[toRow][toCol] != EMPTY)
        return false;

    int capturedRow = (fromRow + toRow) / 2;
    int capturedCol = (fromCol + toCol) / 2;
    Piece opponent = (player == USER || player == USER_KING) ? BOT : USER;
    Piece opponentKing = (player == USER || player == USER_KING) ? BOT_KING : USER_KING;

    return (abs(toRow - fromRow) == 2 && abs(toCol - fromCol) == 2 &&
            (board[capturedRow][capturedCol] == opponent || board[capturedRow][capturedCol] == opponentKing));
}

bool isValidMove(const std::vector<std::vector<Piece>> &board, int fromRow, int fromCol, int toRow, int toCol, Piece player)
{
    if (toRow < 0 || toRow >= BOARD_SIZE || toCol < 0 || toCol >= BOARD_SIZE || board[toRow][toCol] != EMPTY)
        return false;

    int rowDiff = toRow - fromRow;
    int colDiff = abs(toCol - fromCol);

    if (player == USER && rowDiff == -1 && colDiff == 1)
        return true;
    if (player == BOT && rowDiff == 1 && colDiff == 1)
        return true;
    if ((player == USER_KING || player == BOT_KING) && abs(rowDiff) == 1 && colDiff == 1)
        return true;

    return false;
}

std::vector<Move> getPossibleMoves(const std::vector<std::vector<Piece>> &board, Piece player)
{
    std::vector<Move> moves;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if (board[row][col] == player)
            {
                int directions[] = {-1, 1};
                for (int dRow : directions)
                {
                    for (int dCol : directions)
                    {
                        int newRow = row + dRow;
                        int newCol = col + dCol;
                        if (isValidMove(board, row, col, newRow, newCol, player))
                        {
                            moves.push_back({row, col, newRow, newCol, false, -1, -1});
                        }

                        // Check for captures
                        newRow = row + 2 * dRow;
                        newCol = col + 2 * dCol;
                        if (isValidCapture(board, row, col, newRow, newCol, player))
                        {
                            moves.push_back({row, col, newRow, newCol, true, (row + newRow) / 2, (col + newCol) / 2});
                        }
                    }
                }
            }
        }
    }
    return moves;
}

void makeMove(std::vector<std::vector<Piece>> &board, const Move &move)
{
    Piece player = board[move.fromRow][move.fromCol];
    board[move.toRow][move.toCol] = player;
    board[move.fromRow][move.fromCol] = EMPTY;

    if (move.isCapture)
    {
        board[move.capturedRow][move.capturedCol] = EMPTY;
    }

    // Promote to king
    if (player == USER && move.toRow == 0)
    {
        board[move.toRow][move.toCol] = USER_KING;
    }
    else if (player == BOT && move.toRow == BOARD_SIZE - 1)
    {
        board[move.toRow][move.toCol] = BOT_KING;
    }
}

void userTurn(std::vector<std::vector<Piece>> &board)
{
    while (true)
    {
        std::string input;
        std::cout << "Enter your move (fromRow fromCol toRow toCol) or type 'stop' to end: ";
        std::cin >> input;

        // End game if the user types "stop"
        if (input == "stop")
        {
            std::cout << "Game stopped by user.\n";
            exit(0);
        }

        int fromRow, fromCol, toRow, toCol;
        fromRow = std::stoi(input); // Parse the first input as an integer
        std::cin >> fromCol >> toRow >> toCol;

        for (const auto &move : getPossibleMoves(board, USER))
        {
            if (move.fromRow == fromRow && move.fromCol == fromCol && move.toRow == toRow && move.toCol == toCol)
            {
                makeMove(board, move);
                return;
            }
        }
        std::cout << "Invalid move. Try again.\n";
    }
}

void botTurn(std::vector<std::vector<Piece>> &board)
{
    std::vector<Move> moves = getPossibleMoves(board, BOT);
    if (!moves.empty())
    {
        // Prioritize captures
        for (const auto &move : moves)
        {
            if (move.isCapture)
            {
                makeMove(board, move);
                std::cout << "Bot captures from (" << move.fromRow << ", " << move.fromCol
                          << ") to (" << move.toRow << ", " << move.toCol << ")\n";
                return;
            }
        }

        // Otherwise, make a random move
        srand(time(0));
        Move chosenMove = moves[rand() % moves.size()];
        makeMove(board, chosenMove);
        std::cout << "Bot moves from (" << chosenMove.fromRow << ", " << chosenMove.fromCol
                  << ") to (" << chosenMove.toRow << ", " << chosenMove.toCol << ")\n";
    }
    else
    {
        std::cout << "Bot has no valid moves.\n";
    }
}

bool isGameOver(const std::vector<std::vector<Piece>> &board, Piece player)
{
    return getPossibleMoves(board, player).empty();
}

int main()
{
    std::vector<std::vector<Piece>> board(BOARD_SIZE, std::vector<Piece>(BOARD_SIZE, EMPTY));

    // Initialize board with user and bot pieces
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if ((row + col) % 2 == 1)
            {
                if (row < 3)
                    board[row][col] = BOT;
                else if (row > 4)
                    board[row][col] = USER;
            }
        }
    }

    std::cout << "Welcome to Checkers! Type 'stop' anytime to end the game.\n";
    while (true)
    {
        printBoard(board);

        std::cout << "Your turn.\n";
        userTurn(board);

        if (isGameOver(board, BOT))
        {
            std::cout << "You win!\n";
            break;
        }

        printBoard(board);

        std::cout << "Bot's turn.\n";
        botTurn(board);

        if (isGameOver(board, USER))
        {
            std::cout << "Bot wins!\n";
            break;
        }
    }

    return 0;
}
