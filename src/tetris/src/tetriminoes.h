#ifndef TETRIMINOES_H
#define TETRIMINOES_H

typedef enum {
    EMPTY,
    TETR_O,
    TETR_I,
    TETR_S,
    TETR_Z,
    TETR_L,
    TETR_J,
    TETR_T,
    CLEAR,
    FLOATING,
} Tetriminoes_type;

/// Specifies the usual tetrominoes types: O,I,S,Z,L,J,T \n
/// as well as the `EMPTY` tetromino
typedef enum {
    TETROMINO_EMPTY = 0,
    TETROMINO_O = 1,
    TETROMINO_I = 2,
    TETROMINO_S = 3,
    TETROMINO_Z = 4,
    TETROMINO_L = 5,
    TETROMINO_J = 6,
    TETROMINO_T = 7,
} TetrominoType;

// Specifies the 7 tetrominoes and its 4 rotations
// First element in the array is empty
extern int tetriminoes[8][4][4][4];
extern int wall_kicks[8][4][5][2];

void printTetriminoes(TetrominoType piece, int rotation);

#endif //TETRIMINOES_H
