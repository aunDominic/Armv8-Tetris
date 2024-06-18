/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "game.h"
#include "tetriminoes.h"
#include <stdio.h>
//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
#define BLOCK_SIZE 20
#define GRAVITY_TIME 20
#define LIGHTBLUE CLITERAL(Color){0, 229, 255, 255}
#define JBLUE CLITERAL(Color){0, 38, 255, 255}
#define LIMEGREEN CLITERAL(Color){55, 255, 0, 255}
#define SHADOW CLITERAL(Color){51, 51, 51, 150}
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
    if (IsKeyDown(KEY_S)) player1.y += 3.0f;
    else if (IsKeyDown(KEY_W)) player1.y -= 3.0f;
    if (IsKeyDown(KEY_D)) player1.x += 3.0f;
    else if (IsKeyDown(KEY_A)) player1.x -= 3.0f;
    if (framesCounter % GRAVITY_TIME == 0) gravity();


    HandleInput(framesCounter);
    framesCounter++;

    set_shadow();
    // Press enter or tap to change to ENDING screen
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    // {
    //     finishScreen = 1;
    //     PlaySound(fxCoin);
    // }

}
// Gameplay Screen Draw logic
// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
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

    // TODO: Draw GAMEPLAY screen here!

    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    BeginTextureMode(screenCamera);
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
        // Draw full scene with first camera

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
                if (tetriminoes[piece][rotation][i][j] == piece)
                    DrawRectangle(BLOCK_SIZE * xcord, BLOCK_SIZE * ycord, BLOCK_SIZE, BLOCK_SIZE, SHADOW);
            }
        }

        // DRAW GRID LINES
        for (int i = 0; i < COL + 1; i++) {
            DrawLineV((Vector2){(float)BLOCK_SIZE * i, BLOCK_SIZE * 4}, (Vector2){ (float)BLOCK_SIZE * i, (float)BLOCK_SIZE * (ROW + 4)}, LIGHTGRAY);
            DrawText(TextFormat("%i", i), BLOCK_SIZE * i, -100, 10, BLACK);
        }
        DrawText(TextFormat("Current piece:%i", piece), -10, -10, 30, BLACK);
        for (int i = 4; i < ROW + 4 + 1; i++) {
            DrawLineV((Vector2){0, (float)BLOCK_SIZE * i}, (Vector2){(float)BLOCK_SIZE * COL, (float)BLOCK_SIZE * i}, LIGHTGRAY);
            DrawText(TextFormat("%i", i), 0, BLOCK_SIZE * i, 10, BLACK);
        }
        EndMode2D();
    EndTextureMode();

    DrawTextureRec(screenCamera.texture, screenRec, (Vector2){ 0, 0 }, WHITE);

    // Vector2 pos = { 20, 10 };
    // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize * 3.0f, 4, MAROON);
    // DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);
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

    if (IsKeyPressed(KEY_R)) {
        init_board();
    }
}
