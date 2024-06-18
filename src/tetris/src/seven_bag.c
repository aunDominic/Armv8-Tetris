//
// This file uses the 7-bag randomizer system
// using fisher-yates algorithm to implement it
//

#include "seven_bag.h"

#include <stdlib.h>

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    MODULE (INTERNAL) FUNCTIONS
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

/// Shuffles existing bag
void shuffle(int *bag, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }

    // do one further swap with index 0 due to
    // line piece showing up at index 0 too much
    int j = rand() % size;
    int temp = bag[0];
    bag[0] = bag[j];
    bag[j] = temp;
}

/// Generates a new bag by assigning each
/// bag element 1 through 7 (respectively)
void generateNewBag(int *bag) {
    for (int i = 0; i < BAG_SIZE; i++) {
        bag[i] = i + 1;
    }
    shuffle(bag, BAG_SIZE);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\
    HEADER (GLOBAL) FUNCTIONS
//\\\\\\\\\\\\\\\\\\\\\\\\\//

u16_t viewNextFive() {
    return 1;
}

TetrominoType drawFromBag() {
    static int bag[BAG_SIZE];
    static int index = BAG_SIZE;

    if (index >= BAG_SIZE) {
        generateNewBag(bag);
        index = 0;
    }
    return bag[(index)++];
}