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
#define BLOCK_SIZE 20
#define LIGHTBLUE CLITERAL(Color){0, 229, 255, 255}
#define JBLUE CLITERAL(Color){0, 38, 255, 255}
#define LIMEGREEN CLITERAL(Color){55, 255, 0, 255}
#define SHADOW CLITERAL(Color){51, 51, 51, 150}

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

static Rectangle player1 = { 200, 200, BLOCK_SIZE, BLOCK_SIZE };

// Variables to keep track of the state
static int leftKeyFrames = 0;
static int rightKeyFrames = 0;
static bool leftKeyHeld = false;
static bool rightKeyHeld = false;
static bool dasActive = false;

Color tetr_colors[8] = {
        BLACK,
        YELLOW,
        LIGHTBLUE,
        LIMEGREEN,
        RED,
        ORANGE,
        JBLUE,
        PURPLE,
};

//----------------------------------------------------------------------------------
// Gameplay Movement Handling Function Defintions
//----------------------------------------------------------------------------------

// This will handle keyboard inputs and call the appropiate board functions
// ie will handle rotation, move piece horizontally, hard drop, soft drop etc
void HandleInput(int framesCounter);

/// This will draw the provided tetrominoType to whatever thing is currently enabled,
/// with the top-right corner of the tetrominoType being specified with the offsets
static void DrawPiece(int posX, int posY, TetrominoType tetrominoType, TetrominoRotation tetrominoRotation, int blockSize, Color color);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // load render textures
    boardTexture = LoadRenderTexture(COL * BLOCK_SIZE, (ROW + BOARD_START_POS_Y) * BLOCK_SIZE);
    nextFiveTexture = LoadRenderTexture(TETROMINO_SIZE_X * BLOCK_SIZE, 5 * TETROMINO_SIZE_Y * BLOCK_SIZE);
    holdPieceTexture = LoadRenderTexture(TETROMINO_SIZE_X * BLOCK_SIZE, TETROMINO_SIZE_Y * BLOCK_SIZE);
    scoreLevelLinesTexture = LoadRenderTexture(COL * BLOCK_SIZE, 90);
    timerTexture = LoadRenderTexture(COL * BLOCK_SIZE, 30);

    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    init_board();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    // // Press enter or tap to change to ENDING screen
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    // {
    //     finishScreen = 1;
    //     PlaySound(fxCoin);
    // }

    if (IsKeyDown(KEY_S)) player1.y += 3.0f;
    else if (IsKeyDown(KEY_W)) player1.y -= 3.0f;
    if (IsKeyDown(KEY_D)) player1.x += 3.0f;
    else if (IsKeyDown(KEY_A)) player1.x -= 3.0f;

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

    // draw blocks
    for (int i = 0; i < ROW + BOARD_START_POS_Y; i++) {
        for (int j = 0; j < COL; j++) { // X coordinates, col number??
            if (board[i][j] == FLOATING)
                DrawRectangle(BLOCK_SIZE * j, BLOCK_SIZE * i, BLOCK_SIZE, BLOCK_SIZE, tetr_colors[piece]);
            else
                DrawRectangle(BLOCK_SIZE * j, BLOCK_SIZE * i, BLOCK_SIZE, BLOCK_SIZE, tetr_colors[board[i][j]]);
        }
    }

    // draw shadows
    for (int i = 0; i < MAX_PIECE_SIZE; i++) {
        for (int j = 0; j < MAX_PIECE_SIZE; j++) {
            int xcord = shadow_pos.x + j;
            int ycord = shadow_pos.y + i;
            if (tetrominoes[piece][rotation][i][j] == piece)
                DrawRectangle(BLOCK_SIZE * xcord, BLOCK_SIZE * ycord, BLOCK_SIZE, BLOCK_SIZE, SHADOW);
        }
    }

    // draw grid lines
    for (int i = 0; i < COL + 1; i++) {
        DrawLineV((Vector2){(float)BLOCK_SIZE * i, BLOCK_SIZE * 4}, (Vector2){ (float)BLOCK_SIZE * i, (float)BLOCK_SIZE * (ROW + BOARD_START_POS_Y)}, LIGHTGRAY);
    }
    for (int i = BOARD_START_POS_Y; i < ROW + BOARD_START_POS_Y + 1; i++) {
        DrawLineV((Vector2){0, (float)BLOCK_SIZE * i}, (Vector2){(float)BLOCK_SIZE * COL, (float)BLOCK_SIZE * i}, LIGHTGRAY);
    }
    EndTextureMode();

    // Draw the "next five pieces" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(nextFiveTexture);
    ClearBackground(RAYWHITE);

    for (int i = 0; i < 5; i++) {
        DrawPiece(0,i * TETROMINO_SIZE_Y * BLOCK_SIZE,
            next_five_pieces[i],
            0, // default rotation upright
            BLOCK_SIZE,
            tetr_colors[next_five_pieces[i]]);
    }
    EndTextureMode();

    // Draw the "hold piece" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(holdPieceTexture);
    ClearBackground(RAYWHITE);

    if (!can_hold) {
        // if can't hold, draw hold piece with opacity = 33.33%
        const float opacity = 33.33f;
        const Color currentPieceColor = tetr_colors[hold_piece_buffer];
        DrawPiece(0, 0,
            hold_piece_buffer,
            0, // default rotation upright
            BLOCK_SIZE,
            (Color){
                .r = currentPieceColor.r,
                .g = currentPieceColor.g,
                .b = currentPieceColor.b,
                .a = (unsigned char)(opacity / 100 * (float)currentPieceColor.a) });
    } else { // otherwise, draw hold piece with opacity = 100%
        DrawPiece(0, 0,
            hold_piece_buffer,
            0, // default rotation upright
            BLOCK_SIZE,
            tetr_colors[hold_piece_buffer]);
    }
    EndTextureMode();

    // Draw the score, level, and lines cleared to texture
    //---------------------------------------------------------
    BeginTextureMode(scoreLevelLinesTexture);
    ClearBackground(RAYWHITE);

    DrawText(TextFormat("SCORE: %u", score), 0, 0, 20, BLACK);
    DrawText(TextFormat("LINES CLEARED: %u", lines_cleared), 0, 30, 20, BLACK);
    DrawText(TextFormat("LEVEL: %u", level), 0, 60, 20, BLACK);
    EndTextureMode();

    // Draw the "timer" UI element to texture
    //---------------------------------------------------------
    BeginTextureMode(timerTexture);
    ClearBackground(RAYWHITE);

    ElapsedTime elapsedTime = { .integer_encoding = get_elapsed_time() };
    DrawText(TextFormat("TIME: %02d:%02d:%02d", elapsedTime.time.hours, elapsedTime.time.minutes, elapsedTime.time.seconds),
             0, 0, 20, BLACK);
    EndTextureMode();

    // Draw textures to screen, positioning them into a cohesive UI
    //---------------------------------------------------------
    DrawTextureRec(
            boardTexture.texture,
            (Rectangle){
                    .width = (float)boardTexture.texture.width,
                    .height = (float)-boardTexture.texture.height },
            (Vector2){
                .x = (float)holdPieceTexture.texture.width,
                .y = 0 },
            WHITE);

    DrawTextureRec(
            nextFiveTexture.texture,
            (Rectangle){
                    .width = (float)nextFiveTexture.texture.width,
                    .height = (float)-nextFiveTexture.texture.height },
            (Vector2){
                .x = (float)(holdPieceTexture.texture.width + boardTexture.texture.width),
                .y = BOARD_START_POS_Y * BLOCK_SIZE },
            WHITE);

    DrawTextureRec(
            holdPieceTexture.texture,
            (Rectangle){
                    .width = (float)holdPieceTexture.texture.width,
                    .height = (float)-holdPieceTexture.texture.height },
            (Vector2){
                    .x = 0,
                    .y = BOARD_START_POS_Y * BLOCK_SIZE },
            WHITE);

    DrawTextureRec(
            scoreLevelLinesTexture.texture,
            (Rectangle){
                    .width = (float)scoreLevelLinesTexture.texture.width,
                    .height = (float)-scoreLevelLinesTexture.texture.height },
            (Vector2){
                    .x = (float)holdPieceTexture.texture.width,
                    .y = (float)boardTexture.texture.height },
            WHITE);

    DrawTextureRec(
            timerTexture.texture,
            (Rectangle){
                    .width = (float)timerTexture.texture.width,
                    .height = (float)-timerTexture.texture.height },
            (Vector2){
                    .x = (float)holdPieceTexture.texture.width,
                    .y = (float)(boardTexture.texture.height + scoreLevelLinesTexture.texture.height) },
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
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return is_game_over();
}

// only soft_drop needs the framesCounter
// void HandleInput(int framesCounter) {
//     if (IsKeyPressed(KEY_LEFT)) move_piece_left();
//     if (IsKeyPressed(KEY_RIGHT)) move_piece_right();
//     if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_X)) rotate_piece_clockwise();
//     if (IsKeyPressed(KEY_Z)) rotate_piece_counter_clockwise();
//     if (IsKeyPressed(KEY_DOWN)) {
//         soft_drop(framesCounter);
//     } else if (IsKeyPressed(KEY_SPACE)) {
//         hard_drop();
//     }
//
//     if (IsKeyPressed(KEY_C)) {
//         hold_piece();
//     }
//
//     if (IsKeyPressed(KEY_R)) init_board();
// }

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
                if (ARR == 0) {
                    for (int i = 0; i < 10; i++) {
                        move_piece_left();
                    }
                } else if ((leftKeyFrames - DAS) % ARR == 0) {
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

//    if (IsKeyPressed(KEY_R)) {
//        init_board();
//    }
}

static void DrawPiece(
        int posX,
        int posY,
        TetrominoType tetrominoType,
        TetrominoRotation tetrominoRotation,
        int blockSize,
        Color color)
{
    for (int y = 0; y < TETROMINO_SIZE_Y; y++)
        for (int x = 0; x < TETROMINO_SIZE_X; x++) {
            // don't draw empty tetromino values
            if (tetrominoes[(int)tetrominoType][tetrominoRotation][y][x] == TETROMINO_EMPTY) continue;

            DrawRectangle(
                    posX + x * blockSize,
                    posY + y * blockSize,
                    blockSize,
                    blockSize,
                    color);
        }
}