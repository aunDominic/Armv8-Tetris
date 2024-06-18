#include <stdlib.h>

#include "random_piece.h"

#include "seven_bag.h"


// Generates a new piece
TetrominoType generate_piece() {
    return drawFromBag(); // choose between random_gen() vs drawFromBag();
}

// this is used for totally random generation
static TetrominoType random_gen() {
    return (rand() % 7) + 1;
}