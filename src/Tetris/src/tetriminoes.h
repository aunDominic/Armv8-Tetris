
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
// Specifies the 7 tetriminoes and its 4 rotations
// First element in the array is empty
extern int tetriminoes[8][4][4][4];

void printTetriminoes(int piece, int rotation);