#ifndef RANDOM_PIECE_H
#define RANDOM_PIECE_H

#include "types.h"
#include "tetrominoes.h"

/// The next five pieces that will be dropped,
/// encoded as a 16-bit unsigned integer, but conveniently
/// accessed through union properties
///
/// The bits mean the following:
/// - 00-02: the first tetromino type, in the range of 0..7
/// - 03-05: the second tetromino type, in the range of 0..7
/// - 06-08: the third tetromino type, in the range of 0..7
/// - 09-11: the fourth tetromino type, in the range of 0..7
/// - 12-14: the fifth tetromino type, in the range of 0..7
/// - 15: unused
///
/// A piece type of 0 is an empty piece, while 1..7 are the usual O,I,S,Z,L,J,T
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

/// Returns the next five pieces that will be dropped,
/// encoded as a 16-bit unsigned integer.
///
/// The bits mean the following:
/// - 00-02: the first tetromino type, in the range of 0..7
/// - 03-05: the second tetromino type, in the range of 0..7
/// - 06-08: the third tetromino type, in the range of 0..7
/// - 09-11: the fourth tetromino type, in the range of 0..7
/// - 12-14: the fifth tetromino type, in the range of 0..7
/// - 15: unused
///
/// A piece type of 0 is an empty piece, while 1..7 are the usual O,I,S,Z,L,J,T
///
/// They can be conveniently decoded by instantiating the `FivePiecesPreview` union
/// with the returned integer, and then accessing the `.pieces` property
u16_t viewNextFive();

/// Will print the next five pieces to output, encoded as integer
void printNextFive(u16_t nextFive);

/// Function to generate the next piece,
/// which returns an integer in the range of 0..7
/// representing the tetromino type
TetrominoType generate_piece();

#endif //RANDOM_PIECE_H
