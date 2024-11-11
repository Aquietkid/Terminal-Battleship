#include <iostream>
#include "BombLayer.h"
using namespace std;

class GameBoard
{

private:
    int BOARD_SIZE;
    BombLayer* bombLayer;
    int** board;

public:
    static const int EMPTY_STATE = 0;
    static const int OCCUPIED_STATE = 1;

    static const int MISS = 10;
    static const int HIT = 11;
    static const int ALREADY_BOMBED = 12;

    static const int DESTROYER = 20;
    static const int CRUISER = 21;
    static const int CARRIER = 22;

    static const int DESTROYER_SIZE = 2;
    static const int CRUISER_SIZE = 3;
    static const int CARRIER_SIZE = 4;

    static const int RESULT_LOSE = 90;
    static const int RESULT_WIN = 91;
    static const int EXIT = 999;

    static const int SHIPS_PLACED = 1024;

    GameBoard(int BOARD_SIZE)
    {
        this->BOARD_SIZE = BOARD_SIZE;
        board = new int* [this->BOARD_SIZE];
        for (int ii = 0; ii < this->BOARD_SIZE; ii++)
        {
            board[ii] = new int[this->BOARD_SIZE];
            for (int jj = 0; jj < this->BOARD_SIZE; jj++)
            {
                board[ii][jj] = GameBoard::EMPTY_STATE;
            }
        }
        this->bombLayer = new BombLayer(BOARD_SIZE);
    }

    int bomb(const int row, const int col)
    {
        if (bombLayer->getBombedState(row, col) == BombLayer::BOMBED_STATE)
        {
            return ALREADY_BOMBED;
        }
        bombLayer->setAsBombed(row, col);
        if (board[row][col] == DESTROYER || board[row][col] == CRUISER || board[row][col] == CARRIER)
        {
            board[row][col] = GameBoard::ALREADY_BOMBED;
            return HIT;
        }
        board[row][col] = GameBoard::ALREADY_BOMBED;
        return MISS;
    }

    int getShipSize(int shipID) {
        return (shipID == DESTROYER) ? DESTROYER_SIZE :
            (shipID == CRUISER) ? CRUISER_SIZE : CARRIER_SIZE;
    }

    bool placeShip(int shipID, const int headX, const int headY, const int tailX, const int tailY) {
        int shipSize = (shipID == DESTROYER) ? DESTROYER_SIZE :
            (shipID == CRUISER) ? CRUISER_SIZE : CARRIER_SIZE;

        if (!validateShipCoordinates(headX, headY, tailX, tailY, shipSize)) {
            return false;
        }

        // Vertical ship placement
        if (headX == tailX) {
            for (int jj = min(headY, tailY); jj < max(headY, tailY); ++jj) {
                if (board[jj][headX] != GameBoard::EMPTY_STATE) {
                    return false;
                }
            }
            for (int jj = min(headY, tailY); jj < max(headY, tailY); ++jj) {
                board[jj][headX] = shipID;
            }
        }
        // Horizontal ship placement
        else if (headY == tailY) {
            for (int ii = min(headX, tailX); ii < max(headX, tailX); ++ii) {
                if (board[headY][ii] != GameBoard::EMPTY_STATE) {
                    return false;
                }
            }
            for (int ii = min(headX, tailX); ii < max(headX, tailX); ++ii) {
                board[headY][ii] = shipID;
            }
        }

        return true;
    }



    bool validateShipCoordinates(const int headX, const int headY, const int tailX, const int tailY, const int shipSize) {
        if (!((abs(headX - tailX) == shipSize && headY == tailY) ||
            (abs(headY - tailY) == shipSize && headX == tailX))) {
            return false;
        }
        return true;
    }

    char getBoardValue(const int row, const int col) {
        switch (board[row][col])
        {
        case DESTROYER:
            return 'D';
        case CRUISER:
            return 'C';
        case CARRIER:
            return 'A';
        case EMPTY_STATE:
            return '-';
        case ALREADY_BOMBED:
            return '*';
        default:
            return '-';
        }
    }

    char getShipChar(int shipID) {
        switch (shipID)
        {
        case DESTROYER:
            return 'D';
        case CRUISER:
            return 'C';
        case CARRIER:
            return 'A';
        default:
            return '-';
        }
    }

    bool allShipsBombed() {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == DESTROYER || board[i][j] == CRUISER || board[i][j] == CARRIER) {
                    return false;  // At least one ship is not yet fully bombed
                }
            }
        }
        return true;  // All ships have been bombed
    }

    void selectBombCoordinates(int& x, int& y) {
        x = 0;
        y = 0;
        int ch;

        while (true) {
            // Display the board with the current cursor position highlighted
            displayWithCursor(x, y);
            refresh();

            // Get input from the player
            ch = getch();
            switch (ch) {
            case KEY_UP:
                if (x > 0) --x;
                break;
            case KEY_DOWN:
                if (x < BOARD_SIZE - 1) ++x;
                break;
            case KEY_LEFT:
                if (y > 0) --y;
                break;
            case KEY_RIGHT:
                if (y < BOARD_SIZE - 1) ++y;
                break;
            case ' ': // Spacebar to select the coordinates
            case 10:  // Enter key
                return;  // Exit the function with the selected coordinates
            }
        }
    }

    void displayWithCursor(int x, int y) {
        // Display the board with highlighting at (x, y)
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (i == x && j == y) {
                    attron(A_REVERSE);  // Highlight the current position
                }
                printw("%c ", board[i][j]);
                if (i == x && j == y) {
                    attroff(A_REVERSE);
                }
            }
            printw("\n");
        }
    }

    ~GameBoard()
    {
        delete this->bombLayer;
        for (int ii = 0; ii < BOARD_SIZE; ii++)
        {
            delete[] board[ii];
        }
        delete[] board;
    }
};