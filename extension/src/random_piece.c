#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "random_piece.h"

#include "seven_bag.h"

// LOCAL (MODULE) VARIABLES
bool was_initialized = false;


// LOCAL (MODULE) FUNCTION DECLARATIONS

/// this is used for totally random generation
static TetrominoType random_gen();

/// Auto-initializes randomness (if not initialized already)
static void auto_init_randomness(void);


// HEADER (GLOBAL) FUNCTION IMPLEMENTATIONS

u16_t viewNextFive() {
    auto_init_randomness(); // auto-initialize

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
    auto_init_randomness(); // auto-initialize

    const FivePiecesPreview preview = { .integerEncoding = nextFive };

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
    auto_init_randomness(); // auto-initialize

    const TetrominoType piece = drawFromBag(); // choose between random_gen() vs drawFromBag();

    // TODO: remove this later, this is for debugging
    printNextFive(viewNextFive());

    return piece;
}


// MODULE (LOCAL) FUNCTION IMPLEMENTATIONS

static TetrominoType random_gen() {
    return (rand() % 7) + 1;
}

static void auto_init_randomness(void) {
    if (was_initialized) return; // singleton-like initialization of module

    // randomness seed variables
    u64_t a = clock();
    u64_t b = time(NULL);
    u64_t c = getpid();

    // Robert Jenkins' 96 bit Mix Procedure
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);

    // initialize with resulting seed;
    // mark as initialized
    srand(c);
    was_initialized = true;
}