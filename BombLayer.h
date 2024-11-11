#include <iostream>
using namespace std;

class BombLayer {

private: 
    int** bombLayer;
    int size;

public: 
    static const int UNBOMBED_STATE = 0;
    static const int BOMBED_STATE = 1;

    BombLayer(const int BOARD_SIZE) : size(BOARD_SIZE) {
        bombLayer = new int*[BOARD_SIZE];
        for(int ii=0; ii < BOARD_SIZE; ii++) {
            bombLayer[ii] = new int[BOARD_SIZE];
            for(int jj = 0; jj < BOARD_SIZE; jj++) {
                bombLayer[ii][jj] = BombLayer::UNBOMBED_STATE;
            }
        }
    }

    int getBombedState(const int row, const int col) const {
        return bombLayer[row][col];
    }


    void setAsBombed(const int row, const int col) {
        bombLayer[row][col] = BOMBED_STATE;
    }

    void setAsUnbombed(const int row, const int col) {
        bombLayer[row][col] = UNBOMBED_STATE;
    }

    ~BombLayer() {
        for (int ii = 0; ii < size; ii++)
        {
            delete[] bombLayer[ii];
        }
        delete[] bombLayer;
    }


};