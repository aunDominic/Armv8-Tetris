//
// Created by Andrei on 18/06/24.
//
// Houses all (internal) 7-bag randomizer functionality
// using fisher-yates algorithm to implement it
//

#ifndef SEVEN_BAG_H
#define SEVEN_BAG_H
#include "tetrominoes.h"

#define BAG_SIZE 7

/// The bag of tetrominoes, with the first element
/// being the upcoming tetromino
extern TetrominoType bag[BAG_SIZE];

/// Function to draw the next piece from the bag,
/// which returns an integer in the range of 0..7
/// representing the tetromino type
TetrominoType drawFromBag();

#endif //SEVEN_BAG_H
