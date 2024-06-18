//
// Created by Andrei on 18/06/24.
//
// Houses all (internal) 7-bag randomizer functionality
// using fisher-yates algorithm to implement it
//

#ifndef SEVEN_BAG_H
#define SEVEN_BAG_H

#include "types.h"
#include "tetriminoes.h"

#define BAG_SIZE 7

typedef union {
  u16_t integerEncoding: 15;
  struct {
    TetrominoType first: 3;
    TetrominoType second: 3;
    TetrominoType third: 3;
    TetrominoType fourth: 3;
    TetrominoType fifth: 3;
  } pieces;
} FivePiecesPreview;

/// Returns the next five pieces in the bag,
/// encoded as a 16-bit unsigned integer.
///
/// The bits mean the following following:
/// - 0-2: the first tetrimino type, in the range of 0..7
/// - 3-5: the second tetrimino type, in the range of 0..7
/// - 6-8: the third tetrimino type, in the range of 0..7
/// - 9-11: the fourth tetrimino type, in the range of 0..7
/// - 12-14: the fifth tetrimino type, in the range of 0..7
/// - 15: unused
///
/// A piece type of 0 is an empty piece, while 1..7 are the usual O,I,S,Z,L,J,T
///
/// They can be conveniently decoded by instantiating the `FivePiecesPreview` union
/// with the returned integer, and then accessing the `.pieces` property
u16_t viewNextFive();

/// Function to draw the next item from the bag,
/// which returns an integer in the range of 0..7
/// representing the tetrimino type
TetrominoType drawFromBag();

#endif //SEVEN_BAG_H
