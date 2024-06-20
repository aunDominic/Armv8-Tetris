#ifndef GAME_H
#define GAME_H

#define COL 10
#define ROW 20
#define MAX_PIECE_SIZE 4
#define PIECE_START_POS_X (COL / 2)
#define PIECE_START_POS_Y 0
#define BOARD_START_POS_Y 4

#include <stdbool.h>
#include "types.h"
#include "tetrominoes.h"

typedef struct{
    int x;
    int y;
} pair;

/// A union that encodes the current elapsed time as a 16-bit unsigned integer,
/// but conveniently accessed through the other union properties;
///
/// The bits mean the following:
/// - 00-05: the seconds component, in the range 0..59
/// - 06-11: the minutes component, in the range 0..59
/// - 12-13: the minutes component, in the range 0..3
/// - 14-15: unused
typedef union {
    u16_t integer_encoding;
    struct {
        /// the seconds component, in the range 0..59,
        /// i.e. 6-bit unsigned integer component
        u16_t seconds: 6;

        /// the minutes component, in the range 0..59,
        /// i.e. 6-bit unsigned integer component
        u16_t minutes: 6;

        /// the hours component; the current world record is about one hour,
        /// so x4 it to be safe, i.e. in range 0..3,
        /// i.e. 2-bit unsigned integer,
        u16_t hours: 2;
    } time;
} ElapsedTime;

/// A union that encodes the end-of-game statistics as a 64-bit unsigned integer,
/// but conveniently accessed through the other union properties;
///
/// The bits mean the following:
/// - 00-09: the player level, in the range of 0..1023
/// - 10-23: the number of lines cleared, in range 0..16,383
/// - 24-49: the player score, in range 0..67,108,863
/// - 50-63: the elapsed play-time, encoded as an unsigned integer
typedef union {
    u64_t integer_encoding;
    struct {
        /// player level; the current world record is 255,
        /// so x4 it to be safe, i.e. in range 0..1,023,
        /// i.e. 10-bit unsigned integer,
        u64_t level: 10;

        /// lines cleared; is 10 times the level at most,
        /// so x16 it to be safe, i.e. in range 0..16,383,
        /// i.e. 14-bit unsigned integer,
        u64_t lines_cleared: 14;

        /// player score; the current world record is ~16M,
        /// so x4 it to be safe, i.e. in range 0..67,108,863,
        /// i.e. 26-bit unsigned integer,
        u64_t score: 26;

        /// elapsed play-time, encoded as an unsigned integer which
        /// can be decoded by instantiating the `ElapsedTime` union
        u64_t elapsed_time_integer_encoding: 14;
    } stats;
} EndOfGameStats;

extern int board[ROW + BOARD_START_POS_Y][COL];

extern TetrominoType piece;

extern TetrominoType hold_piece_buffer;

extern bool can_hold;

extern TetrominoType next_five_pieces[5];

extern pair piece_pos;

extern pair shadow_pos;

extern int rotation;

/// player level
extern u16_t level;

/// player score
extern u32_t score;

/// lines cleared
extern u16_t lines_cleared;

void handle_gravity(int frames_counter);

/// Top level function called to perform necessary line clears.
void clear_lines(int l, int u);

bool block_is_filled(int i, int j);

/// Checks if it is game over
bool is_game_over(void);

void move_piece_left(void);

void move_piece_right(void);

void rotate_piece_clockwise(void);

void rotate_piece_counter_clockwise(void);

/// Initialises board
void init_board(void);

void hard_drop(void);

void soft_drop(int framesCounter);

/// stores a piece in a temporary buffer, so you can swap your current piece
void hold_piece(void);

/// Sets the shadow position according to the piece position
void set_shadow(void);

/// Returns the current elapsed play-time, encoded as a 16-bit unsigned integer.
///
/// The bits mean the following:
/// - 00-05: the seconds component, in the range 0..59
/// - 06-11: the minutes component, in the range 0..59
/// - 12-13: the minutes component, in the range 0..3
/// - 14-15: unused
///
/// They can be conveniently decoded by instantiating the `ElapsedTime` union
/// with the returned integer, and then accessing the `.time` property
u16_t get_elapsed_time(void);

/// Returns the the end-of-game statistics, encoded as a 64-bit unsigned integer.
///
/// The bits mean the following:
/// - 00-09: the player level, in the range of 0..1023
/// - 10-23: the number of lines cleared, in range 0..16,383
/// - 24-49: the player score, in range 0..67,108,863
/// - 50-63: the elapsed play-time, encoded as an unsigned integer
///
/// They can be conveniently decoded by instantiating the `EndOfGameStats` union
/// with the returned integer, and then accessing the `.stats` property
u64_t get_end_of_game_stats(void);

#endif //GAME_H
