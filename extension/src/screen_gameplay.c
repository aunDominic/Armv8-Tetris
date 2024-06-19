/**********************************************************************************************
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include <stdio.h>

#include "raylib.h"
#include "screens.h"

#include "tetrominoes.h"
#include "game.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

#define BLOCK_SIZE 20
#define LIGHTBLUE CLITERAL(Color){0, 229, 255, 255}
#define JBLUE CLITERAL(Color){0, 38, 255, 255}
#define LIMEGREEN CLITERAL(Color){55, 255, 0, 255}
#define SHADOW CLITERAL(Color){51, 51, 51, 150}

// offsets for drawing the hold piece
#define HOLD_PIECE_OFFSET_X ((-TETROMINO_SIZE_X - 1) * BLOCK_SIZE)
#define HOLD_PIECE_OFFSET_Y (4 * BLOCK_SIZE)

// offsets for drawing the next 5 pieces
#define NEXT_FIVE_OFFSET_X ((COL + 1) * BLOCK_SIZE)
#define NEXT_FIVE_OFFSET_Y (4 * BLOCK_SIZE)

static Rectangle player1 = { 200, 200, BLOCK_SIZE, BLOCK_SIZE };

// DEFINITIONS for DAS MOVEMENT
// Constants for ARR and DAS
// Adjust ARR, DAS, cancelDASOnDirectionChange as you see fit
#define ARR 4  // Automatic Repeat Rate in frames
#define DAS 9 // Delayed Auto Shift in frames

// Boolean to control whether DAS should be canceled when changing directions
static bool cancelDASOnDirectionChange = false;

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

/// This will draw the provided piece to whatever thing is currently enabled,
/// with the top-right corner of the piece being specified with the offsets
void DrawPiece(int posX, int posY, TetrominoType piece, TetrominoRotation rotation, Color color);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void DrawBoard(void);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
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
    // TODO: Draw GAMEPLAY screen here!
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    // Vector2 pos = { 20, 10 };
    // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    // DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

    static Camera2D camera = { 0 };
    camera.target = (Vector2){ player1.x, player1.y };
    camera.offset = (Vector2){ 200.0f, 200.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    static RenderTexture screenCamera;
    static bool isRenderTextureLoaded = false;

    if (!isRenderTextureLoaded) {
        screenCamera = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        isRenderTextureLoaded = true;
    }

    Rectangle screenRec = { 0.0f, 0.0f, (float)screenCamera.texture.width, (float)-screenCamera.texture.height };

    BeginTextureMode(screenCamera);
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);
    // Draw full scene with first camera

    // Draw lines TEMP
    DrawText(TextFormat("LINES CLEARED: %u", lines_cleared), 0, -60, 20, BLACK);
    DrawText(TextFormat("LEVEL: %u", level), 0, -30, 20, BLACK);
    DrawText(TextFormat("SCORE: %u", score), 0, -90, 20, BLACK);

    // DRAW HOLD PIECE
    if (!can_hold) {
        // if can't hold, draw hold piece with opacity = 33.33%
        const float opacity = 33.33;
        const Color currentPieceColor = tetr_colors[hold_piece_buffer];
        DrawPiece(HOLD_PIECE_OFFSET_X,
        HOLD_PIECE_OFFSET_Y,
            hold_piece_buffer,
            0, // default rotation upright
            (Color){
                .r = currentPieceColor.r,
                .g = currentPieceColor.g,
                .b = currentPieceColor.b,
                .a = (unsigned char)(opacity / 100 * (float)currentPieceColor.a) });
    } else { // otherwise, draw hold piece with opacity = 100%
        DrawPiece(HOLD_PIECE_OFFSET_X,
        HOLD_PIECE_OFFSET_Y,
            hold_piece_buffer,
            0, // default rotation upright
            tetr_colors[hold_piece_buffer]);
    }

    // DRAW NEXT 5 PIECES
    for (int i = 0; i < 5; i++) {
        DrawPiece(NEXT_FIVE_OFFSET_X,
            NEXT_FIVE_OFFSET_Y + i * TETROMINO_SIZE_Y * BLOCK_SIZE,
            next_five_pieces[i],
            0, // default rotation upright
            tetr_colors[next_five_pieces[i]]);
    }

    // DRAW BLOCKS
    for (int i = 0; i < ROW + 4; i++) {
        for (int j = 0; j < COL; j++) { // X coordinates, col number??
            if (board[i][j] == FLOATING)
                DrawRectangle(BLOCK_SIZE * j, BLOCK_SIZE * i, BLOCK_SIZE, BLOCK_SIZE, tetr_colors[piece]);
            else
                DrawRectangle(BLOCK_SIZE * j, BLOCK_SIZE * i, BLOCK_SIZE, BLOCK_SIZE, tetr_colors[board[i][j]]);
        }
    }
    // DRAW SHADOWS
    for (int i = 0; i < MAX_PIECE_SIZE; i++) {
        for (int j = 0; j < MAX_PIECE_SIZE; j++) {
            int xcord = shadow_pos.x + j;
            int ycord = shadow_pos.y + i;
            if (tetrominoes[piece][rotation][i][j] == piece)
                DrawRectangle(BLOCK_SIZE * xcord, BLOCK_SIZE * ycord, BLOCK_SIZE, BLOCK_SIZE, SHADOW);
        }
    }

    // DRAW GRID LINES
    for (int i = 0; i < COL + 1; i++) {
        DrawLineV((Vector2){(float)BLOCK_SIZE * i, BLOCK_SIZE * 4}, (Vector2){ (float)BLOCK_SIZE * i, (float)BLOCK_SIZE * (ROW + 4)}, LIGHTGRAY);
    }
    for (int i = 4; i < ROW + 4 + 1; i++) {
        DrawLineV((Vector2){0, (float)BLOCK_SIZE * i}, (Vector2){(float)BLOCK_SIZE * COL, (float)BLOCK_SIZE * i}, LIGHTGRAY);
    }
    EndMode2D();
    EndTextureMode();

    DrawTextureRec(screenCamera.texture, screenRec, (Vector2){ 0, 0 }, WHITE);
}

// // Call this function when you are done with the render texture
// void UnloadGameplayResources(void) {
//     UnloadRenderTexture(screenCamera);
// }

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
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

    if (IsKeyPressed(KEY_R)) {
        init_board();
    }
}

void DrawPiece(
    const int posX,
    const int posY,
    const TetrominoType piece,
    const TetrominoRotation rotation,
    const Color color)
{
    for (int y = 0; y < TETROMINO_SIZE_Y; y++)
        for (int x = 0; x < TETROMINO_SIZE_X; x++) {
            // don't draw empty tetromino values
            if (tetrominoes[(int)piece][rotation][y][x] == TETROMINO_EMPTY) continue;

            DrawRectangle(
                posX + x * BLOCK_SIZE,
                posY + y * BLOCK_SIZE,
                BLOCK_SIZE,
                BLOCK_SIZE,
                color);
        }
}