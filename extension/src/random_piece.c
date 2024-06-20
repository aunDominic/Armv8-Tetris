#include <stdlib.h>
#include <stdio.h>

#include "random_piece.h"

#include "seven_bag.h"

u16_t viewNextFive() {
    // TODO: if other algorithms are used (other than the 7-bag), ensure they also support this feature

    // grab the next 5 pieces from the bag,
    // and construct the `FivePiecesPreview` union
    const FivePiecesPreview preview = { .pieces = {
        .first = bag[0],
        .second = bag[1],
        .third = bag[2],
        .fourth = bag[3],
        .fifth = bag[4]
    } };

    // return the corresponding 16-bit integer
    return preview.integerEncoding;
}

void printNextFive(const u16_t nextFive) {
    const FivePiecesPreview preview = {.integerEncoding = nextFive};

    printf("NEXT FIVE PIECES: ");
    printf("\t1: ");
    printTetrominoType(preview.pieces.first);
    printf("\n");

    printf("\t2: ");
    printTetrominoType(preview.pieces.second);
    printf("\n");

    printf("\t3: ");
    printTetrominoType(preview.pieces.third);
    printf("\n");

    printf("\t4: ");
    printTetrominoType(preview.pieces.fourth);
    printf("\n");

    printf("\t5: ");
    printTetrominoType(preview.pieces.fifth);
    printf("\n");
}

// Generates a new piece
TetrominoType generate_piece() {
    const TetrominoType piece = drawFromBag();  // choose between random_gen() vs drawFromBag();

    // TODO: remove this later, this is for debugging
    printNextFive(viewNextFive());

    return piece;
}

// this is used for totally random generation
static TetrominoType random_gen() {
    return (rand() % 7) + 1;
}