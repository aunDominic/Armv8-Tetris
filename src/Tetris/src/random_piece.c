//
// Created by terp on 17/06/24.
//

#include "random_piece.h"

#include <stdlib.h>

#define BAG_SIZE 7

// This file uses the 7-bag randomizer system
// using fisher-yates algorithm to implement it

/*////////////////////////
    FUNCTION DECLRATIONS
*/////////////////////////
void shuffle(int *bag, int size); // shuffles the bag
void generateNewBag(int *bag);
int drawFromBag();
/* ////////////////////// */


// Generates a new piece
int generate_piece() {
    return drawFromBag(); // choose between random_gen() vs drawFromBag();
}

// this is used for totally random generation
static int random_gen() {
    return (rand() % 7) + 1;
}

// Function to shuffle the bag
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

// Function to generate a new bag
void generateNewBag(int *bag) {
    for (int i = 0; i < BAG_SIZE; i++) {
        bag[i] = i + 1;
    }
    shuffle(bag, BAG_SIZE);
}

// Function to draw from the bag
int drawFromBag() {

    static int bag[BAG_SIZE];
    static int index = BAG_SIZE;

    if (index >= BAG_SIZE) {
        generateNewBag(bag);
        index = 0;
    }
    return bag[(index)++];
}