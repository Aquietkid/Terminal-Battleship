#include <iostream>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include "GameBoard.h"

class VisualBoard {
private:
    int size;
    int cursorX;
    int cursorY;
    const char* player;
    GameBoard* gameBoard;

    void initialize() {
        initscr();              // Initialize the ncurses window
        cbreak();               // Disable line buffering
        keypad(stdscr, TRUE);   // Enable arrow keys
        noecho();               // Don't echo the input
        curs_set(0);            // Hide the default cursor
    }

public:
    VisualBoard(int boardSize, char* player) : size(boardSize), cursorX(0), cursorY(0), player(player) {
        gameBoard = new GameBoard(boardSize);
        initialize();
    }

    void drawBoard(int shipID, bool vertical, const char* message) {
        clear();

        int shipSize = gameBoard->getShipSize(shipID);

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                bool isShip = false;
                if (vertical) {
                    if (j == cursorX && i >= cursorY && i < cursorY + shipSize) {
                        isShip = true;
                    }
                }
                else {
                    if (i == cursorY && j >= cursorX && j < cursorX + shipSize) {
                        isShip = true;
                    }
                }

                if (isShip) {
                    mvprintw(i, j * 2, "%c ", gameBoard->getShipChar(shipID));
                }
                else {
                    mvprintw(i, j * 2, "%c", gameBoard->getBoardValue(i, j));
                }
            }
        }
        printw("\nPlayer: %s\n", player);
        printw("\nX: %d, Y: %d\n", cursorX, cursorY);
        if (message != (char*)"")
            printw("Message: %s\n", message);
        refresh();
    }

    void drawBoard(const char* message) {
        clear();

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (i == cursorY && j == cursorX) {
                    mvprintw(i, j * 2, "X ");
                }
                else
                    mvprintw(i, j * 2, "%c", gameBoard->getBoardValue(i, j));
            }
        }
        printw("\nPlayer: %s\n", player);
        printw("\nX: %d, Y: %d\n", cursorX, cursorY);
        printw("Message: %s\n", message);
        refresh();
    }

    bool placeShip(int shipID) {
        int ch;
        bool vertical = false;
        int shipSize = gameBoard->getShipSize(shipID);

        drawBoard(shipID, vertical, player);

        bool shipPlaced = false;

        while (!shipPlaced)
        {

            while ((ch = getch()) != '\n') {
                switch (ch) {
                case 'r':  // Rotate the ship
                    if ((vertical && cursorX + shipSize <= size) || (!vertical && cursorY + shipSize <= size)) {
                        //check if the ship will remain within the board after rotation
                        vertical = !vertical;
                        break;
                    }
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    moveCursor(ch, shipSize, vertical);
                    break;
                }
                drawBoard(shipID, vertical, player);
            }

            if (vertical) {
                if (cursorY + shipSize <= size) {
                    shipPlaced = gameBoard->placeShip(shipID, cursorX, cursorY, cursorX, cursorY + shipSize);
                }
            }
            else {
                if (cursorX + shipSize <= size) {
                    shipPlaced = gameBoard->placeShip(shipID, cursorX, cursorY, cursorX + shipSize, cursorY);
                }
            }
        }
        return shipPlaced;
    }



    void moveCursor(int key, int shipSize = 0, bool vertical = true) {
        switch (key) {
        case KEY_UP:
            if (cursorY > 0)
                --cursorY;
            break;
        case KEY_DOWN:
            if (vertical && cursorY + shipSize < size) {
                ++cursorY;
            }
            else if (!vertical && cursorY < size - 1) {
                ++cursorY;
            }
            break;
        case KEY_LEFT:
            if (cursorX > 0)
                --cursorX;
            break;
        case KEY_RIGHT:
            if (vertical && cursorX < size - 1) {
                ++cursorX;
            }
            else if (!vertical && cursorX + ((shipSize == 0) ? shipSize + 1 : shipSize) < size) {
                ++cursorX;
            }
            break;
        }
    }


    // void startGame(const int writeEnd, const int readEnd) {
    //     int ch;
    //     int messageNum;
    //     char* message = "";
    //     drawBoard(message);

    //     while ((ch = getch()) != 'q') { // 'q' to quit
    //         if (ch == '\n') {
    //             // int bombed = gameBoard->bomb(cursorY, cursorX);
    //             write(writeEnd, &cursorY, sizeof(int));
    //             write(writeEnd, &cursorX, sizeof(int));

    //             int i, j;
    //             if(read(readEnd, &i, sizeof(int)) > 0) {
    //                 if (read(readEnd, &j, sizeof(int)) > 0) {
    //                     gameBoard->bomb(i, j);
    //                 }
    //             }
    //             else {
    //                 if (messageNum == GameBoard::HIT) {
    //                     // write(writeEnd, &GameBoard::HIT, sizeof(int));
    //                     message = "HIT";
    //                 }
    //                 else if (messageNum == GameBoard::MISS) {
    //                     // write(writeEnd, &GameBoard::MISS, sizeof(int));
    //                     message = "MISS";
    //                 }
    //                 else {
    //                     // write(writeEnd, &GameBoard::ALREADY_BOMBED, sizeof(int));
    //                     message = "ALREADY BOMBED";
    //                 }
    //             }


    //             // printw("\n%d\n", gameBoard->bomb(cursorX, cursorY));
    //             // sleep(2);
    //             // read(readEnd, &messageNum, sizeof(int));
    //             moveCursor(ch);
    //             drawBoard(message);
    //         }
    //     }
    // }

    bool startGame(const int writeEnd, const int readEnd, bool isFirstPlayer) {
        int ch;
        int messageNum;
        char* message = "";
        bool isMyTurn = isFirstPlayer;

        drawBoard(message);

        while (true) {
            if (isMyTurn) {
                drawBoard("Your turn. Press Enter to bomb.");
                while ((ch = getch()) != '\n') {
                    moveCursor(ch);
                    drawBoard("Your turn. Press Enter to bomb.");
                }

                write(writeEnd, &cursorY, sizeof(int));
                write(writeEnd, &cursorX, sizeof(int));

                int bombResult;
                read(readEnd, &bombResult, sizeof(int));

                if (bombResult == GameBoard::HIT) {
                    message = "HIT!";
                }
                else if (bombResult == GameBoard::MISS) {
                    message = "MISS!";
                }
                else if (bombResult == GameBoard::EXIT) {
                    exit(0);
                }
                else {
                    message = "ALREADY BOMBED!";
                }

                printMessage(message, 1, false);

                if (gameBoard->allShipsBombed()) {
                    drawBoard("You win! All ships bombed.");
                    sleep(2);
                    return true;
                }

                int turnCompleteFlag = 1;
                write(writeEnd, &turnCompleteFlag, sizeof(int));

                isMyTurn = false;
            }
            else {
                drawBoard("Opponent's turn. Waiting...");

                int opponentY, opponentX;
                read(readEnd, &opponentY, sizeof(int));
                read(readEnd, &opponentX, sizeof(int));

                int bombResult = gameBoard->bomb(opponentY, opponentX);
                write(writeEnd, &bombResult, sizeof(int));

                if (gameBoard->allShipsBombed()) {
                    drawBoard("You lose! All your ships are bombed.");
                    sleep(2);
                    int exitVal = GameBoard::EXIT;
                    write(writeEnd, &exitVal, sizeof(int));
                    if (player == "1")
                        printMessage("Player 2 wins! ", 5, true);
                    else printMessage("Player 1 wins! ", 5, true);
                    return false;
                }

                int turnCompleteFlag;
                read(readEnd, &turnCompleteFlag, sizeof(int));

                isMyTurn = true;
            }
        }
    }

    void printMessage(const char* message, int sleepFactor, bool toExit) {
        clear();
        printw("%s\n", message);
        refresh();
        sleep(sleepFactor);
        if (toExit)
            exit(0);
    }


    int bomb(const int row, const int col) { //adapter function for bombing
        return gameBoard->bomb(row, col);
    }

    bool allShipsBombed() {
        return gameBoard->allShipsBombed();
    }

    void printMessage(char* message) {
        clear();
        printw(message);
        refresh();
    }


    ~VisualBoard() {
        endwin(); //end ncurses window
        delete gameBoard;
    }
};
