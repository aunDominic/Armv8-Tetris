#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "tetriminoes.h"
#include "game.h"
#include <time.h>
#include <stdlib.h>

/* Board Configuration
                      xcord = WIDTH
    x x x x x x x x x x  - y = 0 - Piece spawn here
    .
    .
    .   
    x x x x x x x x x x  - y = 4 - Board starts here
    .
    .
    .
    x x x x x x x x x x  - y = HEIGHT + 3 - Board ends here
    x = 0
*/

#define START_POS_X (BOARD_WIDTH / 2)
#define START_POS_Y 0


/*////////////////////////
    FUNCTION DECLRATIONS
*/////////////////////////
static void printBoard(); // Print board for debugging purposes
void set_shadow(); // Sets the shadow position according to the piece position
void init_board(); // Initialises board
static bool is_line_filled(int line[]); // Checks if the line is filled duh
static void clear_line(int line[]); // Clears the line (not the same as making it empty)
static void shift_lines_down(int l, int u); // Called after clear line, shifts lines between [l,u] inclusive down 
void clear_lines(int l, int u); // Top level function called to perform necessary line clears. 
void set_piece(); // Places the piece on the board. 
static bool is_valid_orientation(); // Checks if the current orientation of the piece is valid (ie its position and rotation do not collide with the board)
static void clear_draw_piece(); // Clears the current piece in hand from the board
static void redraw_piece(); // Draws the current piece on the screen
/* ////////////////////// */


// Initialises board 
// Extra 4 units of height for pieces to spawn.
// These 4 lines may not be have any pieces placed in them.
int board[BOARD_HEIGHT + 4][BOARD_WIDTH] = {EMPTY}; // Initialise board to empty
int piece = TETR_I; // Current piece in hand
int rotation = 0; // Current rotation.
// To get the current tetrimino
// We use tetriminoes[piece][rotation] -> a 4x4 array containing the tetrimino
pair piece_pos = {.x = START_POS_X, .y = START_POS_Y}; // coordinate of the left corner of the piece
pair shadow_pos = {.x = START_POS_X, .y = START_POS_Y}; // coordinate of left corner of the piece's shadow 


void init_board(void){
    for (int i = 0; i < BOARD_HEIGHT + 4; i++){
        for (int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = EMPTY;
        }
    }
    piece_pos.x = START_POS_X;
    piece_pos.y = START_POS_Y;
    redraw_piece();
    set_shadow();
}
static bool is_line_filled(int line[]){
    for (int i = 0; i < BOARD_WIDTH; i++) if (line[i] == EMPTY) return false;
    return true;
}

static void clear_line(int line[]){
    for (int i = 0; i < BOARD_WIDTH; i++) line[i] = CLEAR;
}

static void shift_lines_down(int l, int u){
   // Naive Gravity. 
   // Lines are shifted down by exactly the number of cleared lines below them
   // The piece is placed between the y coords [l, u], inclusive
   // A higher y value -> a lower position on the board. The y axis points downwards, similar to how 2D Arrays are organised
    assert(l <= u);
    int cnt_clear_lines = 0;
    for (int i = u; i >= l; i--){
        if (board[i][0] == CLEAR) {
            cnt_clear_lines++;
        } 
        else if (cnt_clear_lines > 0){
            for (int j = 0; j < BOARD_WIDTH; j++){
                board[i+cnt_clear_lines][j] = board[i][j];
                board[i][j] = EMPTY; 
            }
            printf("debug: Shifting lines down..\n");
        }
    }
}

// Clears the lines between the y coordinates (l, u)
// l, u are supplied when a piece is placed.
// is called whenever a piece is placed.


void clear_lines(int l, int u){
    assert(l < BOARD_HEIGHT+4 && u < BOARD_HEIGHT+4);
    assert(l < u);

    for (int i = l; i <= u; i++){
        if (is_line_filled(board[i])){
            printf("debug: Clearing lines...\n");
            clear_line(board[i]);
        }
    }
    shift_lines_down(l, u);
}
// void update_piece_shadow(void){
//     for (int i = 0 < MAX_PIECE_SIZE; i++){
//         for (int j = 0; j < MAX_PIECE_SIZE; j++){

//         }
//     }
// }
void set_piece(void){
    printf("debug: Attempting to set piece.\n");
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Fors every row
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // For every column
            if (tetriminoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = piece; 
            }
        }
    }
    clear_lines(0, BOARD_HEIGHT+3);

    printf("debug: Sucessfully set piece. Piece: %d at (%d, %d)\n", piece, piece_pos.x, piece_pos.y);

    piece = (rand() % 7) + 1;
    piece_pos.x = START_POS_X;
    piece_pos.y = START_POS_Y;
    printBoard();
    printTetriminoes(piece, rotation);
    redraw_piece();
}
static void clear_draw_piece(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetriminoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = EMPTY;
            }
        }
    }
}
static void redraw_piece(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetriminoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = FLOATING;
            }
        }
    }
}
static bool is_valid_orientation(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetriminoes[piece][rotation][i][j] == piece &&
                (
                    piece_pos.x + j < 0 
                ||  piece_pos.x + j >= BOARD_WIDTH
                ||  piece_pos.y + i < 0 
                ||  piece_pos.y + i >= BOARD_HEIGHT + 4
                ||  (board[piece_pos.y + i][piece_pos.x + j] != EMPTY) &&
                    board[piece_pos.y + i][piece_pos.x + j] != FLOATING) ){
                return false;
            }
        }
    }
    return true;
}
void move_piece_left(void){
    piece_pos.x--;
    if (!is_valid_orientation()){
        piece_pos.x++;
        return;
    }
    piece_pos.x++;
    clear_draw_piece();
    piece_pos.x--;
    redraw_piece();
}
void move_piece_right(void){
    piece_pos.x++;
    if (!is_valid_orientation()){
        piece_pos.x--;
        return;
    }
    piece_pos.x--;
    clear_draw_piece();
    piece_pos.x++;
    redraw_piece();
}

// void rotate_piece_clockwise(void){
//     int temp = rotation;
//     rotation++;
//     rotation %= 4;
//
//     if (!is_valid_orientation()){
//         rotation = temp;
//         return;
//     }
//     rotation = temp;
//     clear_draw_piece();
//     rotation++; rotation %= 4;
//     redraw_piece();
// }

void rotate_piece_clockwise(void){
    int temp = rotation;
    rotation = (rotation + 1) % 4;
    pair temp_piece_pos = piece_pos;

    if (is_valid_orientation()) {
        rotation = temp;
        clear_draw_piece();
        rotation++; rotation %= 4;
        redraw_piece();
        return;
    }

    // not a valid position so
    // Try the new rotation with wall kicks
    for (int i = 0; i < 5; i++) {
        piece_pos = temp_piece_pos;

        piece_pos.x = piece_pos.x + wall_kicks[piece][rotation][i][0];
        piece_pos.y = piece_pos.y + wall_kicks[piece][rotation][i][1];
        pair new_pos = {piece_pos.x, piece_pos.y};

        if (is_valid_orientation()) {
            printf("Wallkick successful clockwise\n");
            rotation = temp;
            piece_pos = temp_piece_pos;
            clear_draw_piece(); // reset position so we can clear it
            rotation = (rotation + 1) % 4;
            piece_pos = new_pos;
            redraw_piece(); // apply the new thing so we can redraw
            return;
        }
    }

    piece_pos = temp_piece_pos;
    rotation = temp;
    clear_draw_piece();
    redraw_piece();
}

void rotate_piece_counter_clockwise(void){
    int temp = rotation;
    rotation--;
    if (rotation < 0) rotation += 4;
    pair temp_piece_pos = piece_pos;

    if (is_valid_orientation()) {
        rotation = temp;
        clear_draw_piece();
        rotation--;
        if (rotation < 0) rotation += 4;
        redraw_piece();
        return;
    }

    // Try the new rotation with wall kicks
    for (int i = 0; i < 5; i++) {
        piece_pos = temp_piece_pos;

        piece_pos.x = piece_pos.x - wall_kicks[piece][rotation][i][0];
        piece_pos.y = piece_pos.y - wall_kicks[piece][rotation][i][1];
        pair new_pos = {piece_pos.x, piece_pos.y};

        if (is_valid_orientation()) {
            printf("Wallkick successful counter-clockwise\n");
            rotation = temp;
            piece_pos = temp_piece_pos;
            clear_draw_piece(); // set back to previous to clear it
            rotation--;
            if (rotation < 0) rotation += 4;
            piece_pos = new_pos; // reapply changes to draw it
            redraw_piece();
            return;
        }
    }

    piece_pos = temp_piece_pos;
    rotation = temp;
    clear_draw_piece();
    redraw_piece();
}

// void rotate_piece_counter_clockwise(void){
//     int temp = rotation;
//     rotation--;
//     if (rotation < 0) rotation += 4;
//     if (!is_valid_orientation()){
//         rotation = temp;
//         return;
//     }
//     rotation = temp;
//     clear_draw_piece();
//     rotation--;
//     if (rotation < 0) rotation += 4;
//     redraw_piece();
// }

void hard_drop(void){
    clear_draw_piece();
    piece_pos.x = shadow_pos.x;
    piece_pos.y = shadow_pos.y;
    set_piece();
}
void set_shadow(void){
    shadow_pos.x = piece_pos.x;
    for (int y = piece_pos.y; y < BOARD_HEIGHT + 4; y++){
        for (int i = 0; i < MAX_PIECE_SIZE; i++){ // For every row
            for (int j = 0; j < MAX_PIECE_SIZE; j++){ // For every column
                if (tetriminoes[piece][rotation][i][j] == piece && 
                board[y + i][j + piece_pos.x] != EMPTY &&
                board[y + i][j + piece_pos.x] != FLOATING ||
                (tetriminoes[piece][rotation][i][j] == piece && y + i >= BOARD_HEIGHT + 4)){
                    shadow_pos.y = y-1;
                    return;
                }
            }
        }
    }
}
void gravity(void){
    // PRE: The piece currently does not collide with any placed block
    // 
    // If the piece moving the piece down collides with any block, 
    // piece_pos represents the coordinates of the left corner of the piece
    // printf("Attempting gravity\n");
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // For every row
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // For every column
            if (tetriminoes[piece][rotation][i][j] == piece && 
                board[piece_pos.y + i + 1][j + piece_pos.x] != EMPTY &&
                board[piece_pos.y + i + 1][j + piece_pos.x] != FLOATING || 
                (tetriminoes[piece][rotation][i][j] == piece && piece_pos.y + i + 1 >= BOARD_HEIGHT + 4)
               ){
                set_piece();

                return;
            }
        }
    }


    clear_draw_piece();
    piece_pos.y++;
    redraw_piece();
    // printf("Piece has fallen by 1\n");
}

bool block_is_filled(int i, int j){

    return board[i][j] != EMPTY;
}


static void printBoard(){
    printf("New Board State, Current Piece: %d\n", piece);
    for (int i = 3; i < BOARD_HEIGHT + 4; i++){
        for (int j = 0; j < BOARD_WIDTH; j++){
            printf("%d", board[i][j]);
        }
        printf("     y = %d\n", i);
    }
}
// int main(){
//     int i =0;
//     while (i < 50){
//         gravity();
//         // printBoard();
//         clear_lines(0, BOARD_HEIGHT + 3);
//         i ++;
//     }
// }