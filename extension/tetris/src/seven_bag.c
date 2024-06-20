//
// This file uses the 7-bag randomizer system
// using fisher-yates algorithm to implement it
//

#include "seven_bag.h"

#include <stdio.h>
#include <stdlib.h>

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    MODULE (INTERNAL) VARIABLES
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

/// these represent the actual bag which is accessed
/// interactively to produce the very next piece, randomly
static struct {
    size_t cursor;
    TetrominoType pieces[BAG_SIZE];
} actualBag = { .cursor = BAG_SIZE };

/// this represents the next seven pieces that were (or will be)
/// produced by the actual bag - it is guaranteed to not diverge
TetrominoType bag[BAG_SIZE] = { 0 };

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    MODULE (INTERNAL) FUNCTIONS
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

/// Shuffles existing bag
void shuffle(TetrominoType *bag, const int size) {
    for (int i = size - 1; i > 0; i--) {
        const int j = rand() % (i + 1);
        const int temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }

    // do one further swap with index 0 due to
    // line piece showing up at index 0 too much
    const int j = rand() % size;
    const int temp = bag[0];
    bag[0] = bag[j];
    bag[j] = temp;
}

/// Generates a new bag by assigning each
/// bag element 1 through 7 (respectively)
void generateNewBag(TetrominoType bag[BAG_SIZE]) {
    for (int i = 0; i < BAG_SIZE; i++)
        bag[i] = i + 1;
    shuffle(bag, BAG_SIZE);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\
    HEADER (GLOBAL) FUNCTIONS
//\\\\\\\\\\\\\\\\\\\\\\\\\//

TetrominoType drawFromBag() {
    // if any of the facade bag pieces are `TETROMINO_EMPTY`,
    // which should only ever happen at the beginning,
    // (or if something went horribly wrong), then regenerate
    // a new bag
    for (int k = 0; k < BAG_SIZE; k++) {
        if (bag[k] == TETROMINO_EMPTY) {
            generateNewBag(bag); // generate facade bag pieces

            actualBag.cursor = 0; // reset the cursor of the actual bag
            generateNewBag(actualBag.pieces); // generate actual bag pieces
            break;
        }
    }

    // the return value is the first facade bag piece;
    // update the remaining pointers EXCEPT for the last one,
    // which will be updated in the next step
    const TetrominoType returnValue = bag[0];
    for (int i = 0; i < BAG_SIZE - 1; i++)
        bag[i] = bag[i + 1];

    // if we used up the current bag, then
    // reset the cursor and generate a new bag
    if (actualBag.cursor >= BAG_SIZE) {
        actualBag.cursor = 0;
        generateNewBag(actualBag.pieces);
    }

    // updated the last piece by generating it from the actual bag
    bag[BAG_SIZE - 1] = actualBag.pieces[actualBag.cursor++];

    // return the value
    return returnValue;
}

