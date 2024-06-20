/**********************************************************************************************
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

#include "tetrominoes.h"
#include "game.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------

// color macros
#define LIGHTBLUE CLITERAL(Color){0, 229, 255, 255}
#define JBLUE CLITERAL(Color){0, 38, 255, 255}
#define LIMEGREEN CLITERAL(Color){55, 255, 0, 255}
#define SHADOW CLITERAL(Color){51, 51, 51, 150}
#define BORDER_BLUE CLITERAL(Color){100, 210, 255, 150}

// UI element size definitions
#define BORDER_SIZE 7
#define FONT_SIZE 20

#define BOARD_BLOCK_SIZE 25
#define NEXT_FIVE_BLOCK_SIZE 15
#define HOLD_BLOCK_SIZE 15

// DEFINITIONS for DAS MOVEMENT
// Constants for ARR and DAS
// Adjust ARR, DAS, cancelDASOnDirectionChange as you see fit
#define ARR 4  // Automatic Repeat Rate in frames
#define DAS 9 // Delayed Auto Shift in frames

// Render textures for the UI elements
static RenderTexture2D boardTexture = { 0 };
static RenderTexture2D nextFiveTexture = { 0 };
static RenderTexture2D holdPieceTexture = { 0 };
static RenderTexture2D scoreLevelLinesTexture = { 0 };
static RenderTexture2D timerTexture = { 0 };

// Boolean to control whether DAS should be canceled when changing directions
static bool cancelDASOnDirectionChange = false;

static int framesCounter = 0;
static int finishScreen = 0;

// Variables to keep track of the state
static int leftKeyFrames = 0;
static int rightKeyFrames = 0;
static bool leftKeyHeld = false;
static bool rightKeyHeld = false;
static bool dasActive = false;

//----------------------------------------------------------------------------------
// Gameplay Movement Handling Function Defintions
//----------------------------------------------------------------------------------

// This will handle keyboard inputs and call the appropiate board functions
// ie will handle rotation, move piece horizontally, hard drop, soft drop etc
void HandleInput(int framesCounter);

/// This will draw a block of the color corresponding to tetrominoType, to whatever thing is currently enabled
static void DrawBlock(int posX, int posY, TetrominoType tetrominoType, int blockSize, Color tint);

/// This will draw the provided tetrominoType to whatever thing is currently enabled,
/// with the top-right corner of the tetrominoType being specified with the offsets
static void DrawPiece(int posX, int posY, TetrominoType tetrominoType, TetrominoRotation tetrominoRotation, int blockSize, Color tint);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // load render textures
    boardTexture = LoadRenderTexture(
            COL * BOARD_BLOCK_SIZE,
            (ROW + BOARD_START_POS_Y) * BOARD_BLOCK_SIZE);
    nextFiveTexture = LoadRenderTexture(TETROMINO_SIZE_X * NEXT_FIVE_BLOCK_SIZE, 5 * TETROMINO_SIZE_Y * NEXT_FIVE_BLOCK_SIZE);
    holdPieceTexture = LoadRenderTexture(TETROMINO_SIZE_X * HOLD_BLOCK_SIZE, TETROMINO_SIZE_Y * HOLD_BLOCK_SIZE);
    scoreLevelLinesTexture = LoadRenderTexture(100, 160);
    timerTexture = LoadRenderTexture(145, 20);

    PlayMusicStream(music); // play theme music

    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    init_board();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    handle_gravity(framesCounter); // handle gravity

    HandleInput(framesCounter);
    framesCounter++;

    set_shadow();
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // Draw the board to texture
    //---------------------------------------------------------
    BeginTextureMode(boardTexture);
    ClearBackground(RAYWHITE);

    // paint over the starting four lines, with black
    DrawRectangle(0, 0, COL * BOARD_BLOCK_SIZE, BOARD_START_POS_Y * BOARD_BLOCK_SIZE, BLACK);

    // draw blocks in the starting four lines
    for (int i = 0; i < BOARD_START_POS_Y; i++) {
        for (int j = 0; j < COL; j++) { // X coordinates, col number??
            if (board[i][j] == FLOATING)
                DrawBlock(BOARD_BLOCK_SIZE * j,
                          BOARD_BLOCK_SIZE * i,
                          piece,
                          BOARD_BLOCK_SIZE,
                          WHITE);
            else if (board[i][j] != EMPTY)
                DrawBlock(BOARD_BLOCK_SIZE * j,
                          BOARD_BLOCK_SIZE * i,
                          board[i][j],
                          BOARD_BLOCK_SIZE,
                          WHITE);

        }
    }

    // draw blocks in the other lines
    for (int i = BOARD_START_POS_Y; i < ROW + BOARD_START_POS_Y; i++) {
        for (int j = 0; j < COL; j++) { // X coordinates, col number??
            if (board[i][j] == FLOATING)
                DrawBlock(BOARD_BLOCK_SIZE * j,
                          BOARD_BLOCK_SIZE * i,
                          piece,
                          BOARD_BLOCK_SIZE,
                          WHITE);
            else
                DrawBlock(BOARD_BLOCK_SIZE * j,
                          BOARD_BLOCK_SIZE * i,
                          board[i][j],
                          BOARD_BLOCK_SIZE,
                          WHITE);

        }
    }

    // draw shadows
    for (int i = 0; i < MAX_PIECE_SIZE; i++) {
        for (int j = 0; j < MAX_PIECE_SIZE; j++) {
            int xcord = shadow_pos.x + j;
            int ycord = shadow_pos.y + i;
            if (tetrominoes[piece][rotation][i][j] == piece)
                DrawRectangle(BOARD_BLOCK_SIZE * xcord,
                              BOARD_BLOCK_SIZE * ycord,
                              BOARD_BLOCK_SIZE,
                              BOARD_BLOCK_SIZE, SHADOW);
        }
    }

    EndTextureMode();

    // Draw the "next five pieces" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(nextFiveTexture);
    ClearBackground(BLACK);

    for (int i = 0; i < 5; i++) {
        DrawPiece(0, i * TETROMINO_SIZE_Y * NEXT_FIVE_BLOCK_SIZE,
                  next_five_pieces[i],
                  0, // default rotation upright
                  NEXT_FIVE_BLOCK_SIZE,
                  WHITE);
    }
    EndTextureMode();

    // Draw the "hold piece" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(holdPieceTexture);
    ClearBackground(BLACK);

    if (!can_hold) {
        // if can't hold, draw hold piece with opacity = 33.33%
        const float opacity = 33.33f;
        DrawPiece(0, 0,
                  hold_piece_buffer,
                  0, // default rotation upright
            HOLD_BLOCK_SIZE,
                  CLITERAL(Color) { .r=255, .g=255, .b=255, .a=(u8_t)(255*opacity) });
    } else { // otherwise, draw hold piece with opacity = 100%
        DrawPiece(0, 0,
                  hold_piece_buffer,
                  0, // default rotation upright
                  HOLD_BLOCK_SIZE,
                  WHITE);
    }
    EndTextureMode();

    // Draw the score, level, and lines cleared to texture
    //---------------------------------------------------------
    BeginTextureMode(scoreLevelLinesTexture);
    ClearBackground(RAYWHITE);

    int levelY = 0;
    DrawText("LEVEL", 0, levelY, FONT_SIZE, BLACK);
    DrawText(TextFormat("%u", level), 0, levelY + 25, FONT_SIZE, BLACK);

    int scoreY = levelY + 50 + BORDER_SIZE;
    DrawText("SCORE", 0, scoreY, FONT_SIZE, BLACK);
    DrawText(TextFormat("%u", score), 0, scoreY + 25, FONT_SIZE, BLACK);

    int linesY = scoreY + 50 + BORDER_SIZE;
    DrawText("LINES", 0, linesY, FONT_SIZE, BLACK);
    DrawText(TextFormat("%u", lines_cleared), 0, linesY + 25, FONT_SIZE, BLACK);

    EndTextureMode();

    // Draw the "timer" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(timerTexture);
    ClearBackground(RAYWHITE);

    ElapsedTime elapsedTime = { .integer_encoding = get_elapsed_time() };
    DrawText(TextFormat("TIME: %02d:%02d:%02d", elapsedTime.time.hours, elapsedTime.time.minutes, elapsedTime.time.seconds),
             0, 0, FONT_SIZE, BLACK);
    EndTextureMode();

    // Draw textures to screen, positioning them into a cohesive UI
    //---------------------------------------------------------
    ClearBackground(RAYWHITE);

    // draw the board and its border
    int boardX = (GetScreenWidth() - boardTexture.texture.width) / 2;
    int boardY = (GetScreenHeight() - boardTexture.texture.height) / 2;
    DrawRectangleRounded(
            (Rectangle){
                .x = (float)boardX - BORDER_SIZE,
                .y = (float)boardY - BORDER_SIZE,
                .width = (float)boardTexture.texture.width + 2 * BORDER_SIZE,
                .height = (float)boardTexture.texture.height + 2 * BORDER_SIZE },
                0.05f,
                100,
                BORDER_BLUE);
    DrawRectangle(
            boardX,
            boardY,
            boardTexture.texture.width,
            boardTexture.texture.height,
            RAYWHITE);
    DrawTextureRec(
            boardTexture.texture,
            (Rectangle){
                    .width = (float)boardTexture.texture.width,
                    .height = (float)-boardTexture.texture.height },
            (Vector2){ .x = (float)boardX, .y = (float)boardY },
            WHITE);

    // draw the "next five" ui element with its border
    int nextFiveX = ((boardX + boardTexture.texture.width + BORDER_SIZE) + GetScreenWidth() - nextFiveTexture.texture.width) / 2;
    int nextFiveY = boardY + BOARD_START_POS_Y * BOARD_BLOCK_SIZE;
    DrawRectangleRounded(
            (Rectangle){
                    .x = (float)nextFiveX - BORDER_SIZE,
                    .y = (float)nextFiveY - BORDER_SIZE,
                    .width = (float)nextFiveTexture.texture.width + 2 * BORDER_SIZE,
                    .height = (float)nextFiveTexture.texture.height + 2 * BORDER_SIZE },
            0.05f,
            100,
            GRAY);
    DrawTextureRec(
            nextFiveTexture.texture,
            (Rectangle){
                    .width = (float)nextFiveTexture.texture.width,
                    .height = (float)-nextFiveTexture.texture.height },
            (Vector2){
                .x = (float)nextFiveX,
                .y = (float)nextFiveY },
            WHITE);

    // draw the "hold piece" ui element with its border
    int holdPieceX = (boardX - BORDER_SIZE - nextFiveTexture.texture.width) / 2;
    int holdPieceY = boardY + BOARD_START_POS_Y * BOARD_BLOCK_SIZE;
    DrawRectangleRounded(
            (Rectangle){
                    .x = (float)holdPieceX - BORDER_SIZE,
                    .y = (float)holdPieceY - BORDER_SIZE,
                    .width = (float)holdPieceTexture.texture.width + 2 * BORDER_SIZE,
                    .height = (float)holdPieceTexture.texture.height + 2 * BORDER_SIZE },
            0.05f,
            100,
            GRAY);
    DrawTextureRec(
            holdPieceTexture.texture,
            (Rectangle){
                    .width = (float)holdPieceTexture.texture.width,
                    .height = (float)-holdPieceTexture.texture.height },
            (Vector2){
                    .x = (float)holdPieceX,
                    .y = (float)holdPieceY },
            WHITE);

    // draw the level, score, and lines cleared
    int scoreLevelLinesX = (boardX - BORDER_SIZE - scoreLevelLinesTexture.texture.width) / 2;
    int scoreLevelLinesY = boardY + boardTexture.texture.height - scoreLevelLinesTexture.texture.height;
    DrawTextureRec(
            scoreLevelLinesTexture.texture,
            (Rectangle){
                    .width = (float)scoreLevelLinesTexture.texture.width,
                    .height = (float)-scoreLevelLinesTexture.texture.height },
            (Vector2){
                    .x = (float)scoreLevelLinesX,
                    .y = (float)scoreLevelLinesY },
            WHITE);

    // draw timer right above the board
    int timerX = boardX + (boardTexture.texture.width - timerTexture.texture.width) / 2;
    int timerY = boardY - BORDER_SIZE - timerTexture.texture.height - BORDER_SIZE;
    DrawTextureRec(
            timerTexture.texture,
            (Rectangle){
                    .width = (float)timerTexture.texture.width,
                    .height = (float)-timerTexture.texture.height },
            (Vector2){
                    .x = (float)timerX,
                    .y = (float)timerY },
            WHITE);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // unload textures
    UnloadRenderTexture(boardTexture);
    UnloadRenderTexture(nextFiveTexture);
    UnloadRenderTexture(holdPieceTexture);
    UnloadRenderTexture(scoreLevelLinesTexture);
    UnloadRenderTexture(timerTexture);

    // stop playing theme music
    StopMusicStream(music);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return is_game_over();
}

void HandleInput(int framesCounter) {
    // Handle left movement
    if (IsKeyPressed(KEY_LEFT)) {
        move_piece_left();
        leftKeyFrames = 0;
        leftKeyHeld = true;
        rightKeyHeld = false;
        dasActive = false;
    } else if (IsKeyDown(KEY_LEFT)) {
        if (leftKeyHeld) {
            leftKeyFrames++;
            if (leftKeyFrames >= DAS) {
                dasActive = true;
                if ((leftKeyFrames - DAS) % ARR == 0) {
                    move_piece_left();
                }
            }
        }
    } else {
        leftKeyHeld = false;
        leftKeyFrames = 0;
    }

    // Handle right movement
    if (IsKeyPressed(KEY_RIGHT)) {
        // handle special case so it zooms over to the right or left similar to Tetrio
        move_piece_right();
        rightKeyFrames = 0;
        rightKeyHeld = true;
        leftKeyHeld = false;
        dasActive = false;
    } else if (IsKeyDown(KEY_RIGHT)) {
        if (rightKeyHeld) {
            rightKeyFrames++;
            if (rightKeyFrames >= DAS) {
                dasActive = true;
                // handle special case so it zooms over to the right or left similar to Tetrio
                if (ARR == 0) {
                    for (int i = 0; i < COL; i++) {
                        move_piece_right();
                    }
                } else if ((rightKeyFrames - DAS) % ARR == 0) {
                    move_piece_right();
                }
            }
        }
    } else {
        rightKeyHeld = false;
        rightKeyFrames = 0;
    }

    // Cancel DAS if changing directions
    if (cancelDASOnDirectionChange) {
        if (leftKeyHeld && IsKeyPressed(KEY_RIGHT)) {
            leftKeyHeld = false;
            leftKeyFrames = 0;
        }
        if (rightKeyHeld && IsKeyPressed(KEY_LEFT)) {
            rightKeyHeld = false;
            rightKeyFrames = 0;
        }
    }

    // Handle other inputs
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_X)) rotate_piece_clockwise();
    if (IsKeyPressed(KEY_Z)) rotate_piece_counter_clockwise();
    if (IsKeyDown(KEY_DOWN)) {
        soft_drop(framesCounter);
    } else if (IsKeyPressed(KEY_SPACE)) {
        hard_drop();
    }

    if (IsKeyPressed(KEY_C)) {
        hold_piece();
    }
}

static void DrawBlock(int posX, int posY, TetrominoType tetrominoType, int blockSize, Color tint) {
    DrawTexturePro(blocksTexture,
                   (Rectangle){ .x = (float)(5 * tetrominoType), .y = 0, .width = 5, .height = 5 },
                   (Rectangle){ .x = (float)posX, .y = (float)posY, .width = (float)blockSize, .height = (float)blockSize },
                   (Vector2){ 0 },
                   0,
                   tint);
}

static void DrawPiece(
        int posX,
        int posY,
        TetrominoType tetrominoType,
        TetrominoRotation tetrominoRotation,
        int blockSize,
        Color tint)
{
    for (int y = 0; y < TETROMINO_SIZE_Y; y++)
        for (int x = 0; x < TETROMINO_SIZE_X; x++) {
            TetrominoType blockType = tetrominoes[(int)tetrominoType][tetrominoRotation][y][x];
            Color blockTint = blockType == EMPTY ? WHITE : tint; // don't tint empty blocks
            DrawBlock(posX + x * blockSize,
                      posY + y * blockSize,
                      blockType,
                      blockSize,
                      blockTint);
        }
}