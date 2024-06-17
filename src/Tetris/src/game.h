#define COL 10
#define ROW 20
#define MAX_PIECE_SIZE 4

typedef struct{
    int x;
    int y;
} pair;


extern int board[ROW + 4][COL];

extern int piece;

extern pair piece_pos;

extern pair shadow_pos;

extern int rotation;

void gravity(void);

void clear_lines(int l, int u);

bool block_is_filled(int i, int j);

void move_piece_left(void);

void move_piece_right(void);

void rotate_piece_clockwise(void);

void rotate_piece_counter_clockwise(void);

void init_board(void);

void hard_drop(void);

void soft_drop(void);

// stores a piece in a temporary buffer so you can swap your current piece
void hold_piece(void);

void set_shadow(void);

