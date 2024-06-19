#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "tetrominoes.h"
#include "random_piece.h"
#include "game.h"

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

#define LEVEL_UP_THRESHOLD 10

/*////////////////////////
    FUNCTION DECLRATIONS
*/////////////////////////
static void printBoard(); // Print board for debugging purposes

static bool is_line_filled(const int line[]); // Checks if the line is filled duh
static void clear_line(int line[]); // Clears the line (not the same as making it empty)
static void shift_lines_down(int l, int u); // Called after clear line, shifts lines between [l,u] inclusive down
static void update_score(int lines);
void set_piece(); // Places the piece on the board.
static bool is_valid_orientation(); // Checks if the current orientation of the piece is valid (ie its position and rotation do not collide with the board)
static void clear_draw_piece(); // Clears the current piece in hand from the board
static void redraw_piece(); // Draws the current piece on the screen
static TetrominoType get_next_piece(); // updates the `next_five_pieces` buffer and returns the upcoming piece
static void gravity(void);
/* ////////////////////// */


// Initialises board
// Extra 4 units of height for pieces to spawn.
// These 4 lines may not be have any pieces placed in them.
int board[ROW + BOARD_START_POS_Y][COL] = {EMPTY}; // Initialise board to empty
TetrominoType piece = TETROMINO_I; // Current piece in hand
TetrominoType hold_piece_buffer = 0; // set as 0 by default
TetrominoType next_five_pieces[5] = { 0 }; // initialized to 0
int rotation = 0; // Current rotation.
// To get the current tetrimino
// We use tetrominoes[piece][rotation] -> a 4x4 array containing the tetrimino
pair piece_pos = {.x = PIECE_START_POS_X, .y = PIECE_START_POS_Y}; // coordinate of the left corner of the piece
pair shadow_pos = {.x = PIECE_START_POS_X, .y = PIECE_START_POS_Y}; // coordinate of left corner of the piece's shadow

// this is used so people can't infinitely spam the hold button to get extra time;
// can_hold resets after every successful piece placement
bool can_hold = true;

u16_t level = 1;
u32_t score = 0; // layer score
u16_t lines_cleared = 0; // number of lines cleared


// MODULE (LOCAL) VARIABLES

/// variable that holds the unix timestamp (in seconds)
/// of the initialization moment, of the most recent game
static i64_t game_init_timestamp = -1; // negative means uninitialized


// FUNCTION IMPLEMENTATIONS

void init_board(void){
    time(&game_init_timestamp); // set initialization timestamp

    for (int i = 0; i < ROW + BOARD_START_POS_Y; i++){
        for (int j = 0; j < COL; j++){
            board[i][j] = EMPTY;
        }
    }
    piece = get_next_piece();
    rotation = 0;
    piece_pos.x = PIECE_START_POS_X;
    piece_pos.y = PIECE_START_POS_Y;
    redraw_piece();
    set_shadow();
}

static void print_binary(u64_t number)
{
    if (number >> 1) {
        print_binary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

u16_t get_elapsed_time(void) {
    // TEST:
    EndOfGameStats eogse = { .stats = {
            .level = 994,
            .lines_cleared = 9948,
            .score = 50100100,
            .elapsed_time_integer_encoding = ((ElapsedTime){ .time = {
                    .seconds = 49,
                    .minutes = 39,
                    .hours = 2
            } }).integer_encoding
    } };


    print_binary(eogse.integer_encoding);
    printf("\n");
    printf("%lu\n", eogse.integer_encoding);

    assert(game_init_timestamp > 0); // make sure it is initialized

    // get elapsed seconds
    time_t seconds = time(NULL) - game_init_timestamp;

    // compute hours, and update elapsed seconds accordingly
    u8_t hours = seconds / (60 * 60);
    seconds -= hours * 60 * 60;

    // compute minutes, and update elapsed seconds accordingly
    u8_t minutes = seconds / 60;
    seconds -= minutes * 60;

    // create struct `ElapsedTime` union to encode into unsigned integer
    ElapsedTime elapsed_time = { .time = {
            .seconds = seconds,
            .minutes = minutes,
            .hours = hours,
    } };

    // return the integer encoding
    return elapsed_time.integer_encoding;
}

static bool is_line_filled(const int line[]){
    for (int i = 0; i < COL; i++) if (line[i] == EMPTY) return false;
    return true;
}

static void clear_line(int line[]){
    for (int i = 0; i < COL; i++) line[i] = CLEAR;
}

static void update_score(int lines) {
    switch (lines) {
        case 1: score += 100 * level; break;
        case 2: score += 300 * level; break;
        case 3: score += 500 * level; break;
        default: score += 800 * level; // tetris
    }
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
            for (int j = 0; j < COL; j++){
                board[i+cnt_clear_lines][j] = board[i][j];
                board[i][j] = EMPTY;
            }
            printf("debug: Shifting lines down..\n");
        }
    }
    if (cnt_clear_lines > 0)
        update_score(cnt_clear_lines); // update score
    lines_cleared += cnt_clear_lines; // update lines cleared
}

// Clears the lines between the y coordinates (l, u)
// l, u are supplied when a piece is placed.
// is called whenever a piece is placed.


void clear_lines(int l, int u){
    assert(l < ROW+BOARD_START_POS_Y && u < ROW+BOARD_START_POS_Y);
    assert(l < u);

    for (int i = l; i <= u; i++){
        if (is_line_filled(board[i])){
            printf("debug: Clearing lines...\n");
            clear_line(board[i]);
        }
    }
    shift_lines_down(l, u);

    // update the level according to lines cleared
    level = lines_cleared / LEVEL_UP_THRESHOLD + 1;
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
            if (tetrominoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = piece;
            }
        }
    }
    clear_lines(0, ROW+3);

    printf("debug: Sucessfully set piece. Piece: %d at (%d, %d)\n", piece, piece_pos.x, piece_pos.y);

    // code for generating piece and setting it in right place
    // sidenote: some of this code might be needed to implement hold_piece
    piece = get_next_piece();
    piece_pos.x = PIECE_START_POS_X;
    piece_pos.y = PIECE_START_POS_Y;
    rotation = 0;
    can_hold = true;

    printBoard();
    printTetrominoes(piece, rotation);
    redraw_piece();
}
static void clear_draw_piece(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetrominoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = EMPTY;
            }
        }
    }
}
static void redraw_piece(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetrominoes[piece][rotation][i][j] == piece){
                board[piece_pos.y + i][j + piece_pos.x] = FLOATING;
            }
        }
    }
}
static bool is_valid_orientation(void){
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // Piece Height
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // Piece Width
            if (tetrominoes[piece][rotation][i][j] == piece &&
                (
                    piece_pos.x + j < 0
                ||  piece_pos.x + j >= COL
                ||  piece_pos.y + i < 0
                ||  piece_pos.y + i >= ROW + BOARD_START_POS_Y
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

void hard_drop(void){
    clear_draw_piece();
    piece_pos.x = shadow_pos.x;
    piece_pos.y = shadow_pos.y;
    set_piece();
}

void soft_drop(int framesCounter) {
    static int SOFT_DROP_RATE = 5; // adjust this value as you see fit

    if (framesCounter % SOFT_DROP_RATE == 0) {
        gravity();
    }
}

void set_shadow(void){
    shadow_pos.x = piece_pos.x;
    for (int y = piece_pos.y; y < ROW + BOARD_START_POS_Y; y++){
        for (int i = 0; i < MAX_PIECE_SIZE; i++){ // For every row
            for (int j = 0; j < MAX_PIECE_SIZE; j++){ // For every column
                if (tetrominoes[piece][rotation][i][j] == piece &&
                board[y + i][j + piece_pos.x] != EMPTY &&
                board[y + i][j + piece_pos.x] != FLOATING ||
                (tetrominoes[piece][rotation][i][j] == piece && y + i >= ROW + BOARD_START_POS_Y)){
                    shadow_pos.y = y-1;
                    return;
                }
            }
        }
    }
}

static void gravity(void){
    // PRE: The piece currently does not collide with any placed block
    //
    // If the piece moving the piece down collides with any block,
    // piece_pos represents the coordinates of the left corner of the piece
    // printf("Attempting gravity\n");
    for (int i = 0; i < MAX_PIECE_SIZE; i++){ // For every row
        for (int j = 0; j < MAX_PIECE_SIZE; j++){ // For every column
            if (tetrominoes[piece][rotation][i][j] == piece &&
                board[piece_pos.y + i + 1][j + piece_pos.x] != EMPTY &&
                board[piece_pos.y + i + 1][j + piece_pos.x] != FLOATING ||
                (tetrominoes[piece][rotation][i][j] == piece && piece_pos.y + i + 1 >= ROW + BOARD_START_POS_Y)
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

// hold pieces in a temporary buffer, probably a global variable defined in game.h
void hold_piece(void) {
    // swaps current piece and piece in buffer
    // then sets piece to top of board again

    if (!can_hold) {
        return;
    }

    // handle case if hold_piece_buffer doesn't hold a piece ie at the start
    if (hold_piece_buffer == 0) {
        clear_draw_piece();
        hold_piece_buffer = piece;
        piece = get_next_piece();
    } else {
        // hold_piece is not 0 so it actually has a valid piece stored
        clear_draw_piece();
        int temp = piece;
        piece = hold_piece_buffer;
        hold_piece_buffer = temp;
    }

    rotation = 0;
    piece_pos.x = PIECE_START_POS_X;
    piece_pos.y = PIECE_START_POS_Y;
    can_hold = false;
}

bool block_is_filled(int i, int j){
    return board[i][j] != EMPTY;
}

bool is_game_over(void) {
    for (int column = 0; column < COL; column++) {
        // check for the line above the beginning of the board, for game over
        switch ((Tetrominoes_type)board[BOARD_START_POS_Y - 1][column]) {
            case EMPTY: case CLEAR: case FLOATING: continue;
            default: return true;
        }
    }
    return false;
}

static void printBoard(){
    printf("New Board State, Current Piece: %d\n", piece);
    for (int i = 3; i < ROW + BOARD_START_POS_Y; i++){
        for (int j = 0; j < COL; j++){
            printf("%d", board[i][j]);
        }
        printf("     y = %d\n", i);
    }
}

static TetrominoType get_next_piece() {
    // get the next piece
    const TetrominoType piece = generate_piece();

    // grab the next five pieces and decode
    const FivePiecesPreview preview = { .integerEncoding = viewNextFive() };

    // update the buffer appropriately
    next_five_pieces[0] = preview.pieces.first;
    next_five_pieces[1] = preview.pieces.second;
    next_five_pieces[2] = preview.pieces.third;
    next_five_pieces[3] = preview.pieces.fourth;
    next_five_pieces[4] = preview.pieces.fifth;

    // return the next piece
    return piece;
}

void handle_gravity(int frames_counter) {
    // pick correct gravity time, according to the current level
    u32_t gravity_time;
    switch (level) {
        case 1: gravity_time = 20; break;
        case 2: gravity_time = 18; break;
        case 3: gravity_time = 16; break;
        case 4: gravity_time = 14; break;
        case 5: gravity_time = 12; break;
        case 6: gravity_time = 11; break;
        case 7: gravity_time = 10; break;
        case 8: gravity_time = 9; break;
        case 9: gravity_time = 8; break;
        case 10: gravity_time = 7; break;
        case 11: gravity_time = 6; break;
        case 12: gravity_time = 5; break;
        case 13: gravity_time = 4; break;
        case 14: gravity_time = 3; break;
        default: gravity_time = 2;
    }

    if (frames_counter % gravity_time == 0)
        gravity();
}

u64_t get_end_of_game_stats(void) {
    // TODO: implement
    return 0;
}