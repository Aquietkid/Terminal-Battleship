// #include "VisualBoard.h"
// #include <unistd.h>
// #include <sys/ipc.h>
// #include <stdlib.h>
// #include <cstdlib>
// #include <sys/wait.h>

// #define BOARD_SIZE 5

// using namespace std;

// int main(int argc, char* argv[])
// {

//     // set up pipes for communication
//     int pipe1[2], pipe2[2];
//     if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
//     {
//         perror("Error while creating pipes! \n");
//         exit(-1);
//     }

//     // fork two processes

//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("Error while forking! \n");
//         exit(-1);
//     }
//     else if (pid == 0) { //child
//         close(pipe1[0]); //Reading end of pipe 1
//         close(pipe2[1]); //Writing end of pipe 2

//         VisualBoard* board1 = new VisualBoard(BOARD_SIZE, "1");
//         board1->placeShip(GameBoard::DESTROYER);
//         board1->placeShip(GameBoard::CRUISER);
//         board1->placeShip(GameBoard::CARRIER);

//         int* temp = new int;
//         write(pipe1[1], (int*) GameBoard::SHIPS_PLACED, sizeof(int));

//         read(pipe2[0], temp, sizeof(int));

//         board1->startGame(pipe1[1], pipe2[0]);

//         delete board1;
//         // place ships on board 1

//     }
//     else { //parent
//         pid_t pid2 = fork();
//         if (pid2 == -1) {
//             perror("Error while forking! \n");
//             exit(-1);
//         }
//         else if (pid2 == 0) { //child
//             close(pipe1[1]); //Writing end of pipe 1
//             close(pipe2[0]); //Reading end of pipe 2

//             VisualBoard* board2 = new VisualBoard(BOARD_SIZE, "2");

//             int* temp = new int;
//             read(pipe1[0], temp, sizeof(int));
//             write(pipe2[1], (int*) GameBoard::SHIPS_PLACED, sizeof(int));

//             board2->placeShip(GameBoard::DESTROYER);
//             board2->placeShip(GameBoard::CRUISER);
//             board2->placeShip(GameBoard::CARRIER);
//             board2->startGame(pipe2[1], pipe1[0]);

//             delete board2;
//             // place ships on board 2

//         }
//         else { //parent
//             waitpid(pid, NULL, 0);
//             waitpid(pid2, NULL, 0);

//         }

//     }


// }





#include "VisualBoard.h"
#include <unistd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/wait.h>

#define BOARD_SIZE 5

using namespace std;

int main(int argc, char* argv[])
{
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Error while creating pipes!\n");
        exit(-1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error while forking!\n");
        exit(-1);
    }
    else if (pid == 0) {
        close(pipe1[0]);
        close(pipe2[1]);

        VisualBoard* board1 = new VisualBoard(BOARD_SIZE, "1");
        board1->placeShip(GameBoard::DESTROYER);
        board1->placeShip(GameBoard::CRUISER);
        board1->placeShip(GameBoard::CARRIER);

        int signal = GameBoard::SHIPS_PLACED;
        write(pipe1[1], &signal, sizeof(int));
        read(pipe2[0], &signal, sizeof(int));

        if (board1->startGame(pipe1[1], pipe2[0], true)) {
            delete board1;
            close(pipe1[1]);
            close(pipe2[0]);
            exit(1);
        }

        delete board1;
        close(pipe1[1]);
        close(pipe2[0]);

        exit(0);
    }
    else {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("Error while forking!\n");
            exit(-1);
        }
        else if (pid2 == 0) {
            close(pipe1[1]);
            close(pipe2[0]);

            VisualBoard* board2 = new VisualBoard(BOARD_SIZE, "2");

            int signal;
            read(pipe1[0], &signal, sizeof(int));

            board2->placeShip(GameBoard::DESTROYER);
            board2->placeShip(GameBoard::CRUISER);
            board2->placeShip(GameBoard::CARRIER);

            signal = GameBoard::SHIPS_PLACED;
            write(pipe2[1], &signal, sizeof(int));


            if (board2->startGame(pipe2[1], pipe1[0], false)) {
                delete board2;
                close(pipe1[0]);
                close(pipe2[1]);
                exit(1);
            }

            delete board2;
            close(pipe1[0]);
            close(pipe2[1]);

            exit(0);
        }
        else {
            int p1stat, p2stat;
            waitpid(pid, &p1stat, 0);
            waitpid(pid2, &p2stat, 0);

            // char temp[BUFSIZ];
            // read(pipe1[0], temp, BUFSIZ);
            // cout << temp << endl;
            // read(pipe2[0], temp, BUFSIZ);
            // cout << temp << endl;

            close(pipe1[1]);
            close(pipe1[0]);
            close(pipe2[1]);
            close(pipe2[0]);
        }
    }

    return 0;
}
