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

extern int board[ROW + BOARD_START_POS_Y][COL];

extern TetrominoType piece;

extern TetrominoType hold_piece_buffer;

extern bool can_hold;

extern TetrominoType next_five_pieces[5];

extern pair piece_pos;

extern pair shadow_pos;

extern int rotation;

extern u32_t level;
extern u64_t score;
extern u32_t lines_cleared;

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

#endif //GAME_H
