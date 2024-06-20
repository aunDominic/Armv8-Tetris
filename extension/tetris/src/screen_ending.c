/**********************************************************************************************
*
*   Ending Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "game.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    PlaySound(fxGameOver); // play game over sound

    // TODO: Initialize ENDING screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{
    // TODO: Update ENDING screen variables here!

    // Press enter or tap to return to TITLE screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
    }
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    // TODO: Draw ENDING screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);

    DrawTextEx(font, "GAME OVER", (Vector2){ 20, 10 }, font.baseSize*3.0f, 4, DARKBLUE);
    DrawText("PRESS ENTER or", 165, 320, 20, DARKBLUE);
    DrawText("TAP to PLAY AGAIN", 150, 345, 20, DARKBLUE);

    // grab end-of-game statistics, and timer
    EndOfGameStats endOfGameStats = { .integer_encoding = get_end_of_game_stats() };
    ElapsedTime elapsedTime = { .integer_encoding = endOfGameStats.stats.elapsed_time_integer_encoding };
    DrawText(TextFormat("TIME: %02d:%02d:%02d", elapsedTime.time.hours, elapsedTime.time.minutes, elapsedTime.time.seconds),
             20, 60, 20, BLACK);
    DrawText(TextFormat("LEVEL: %u", endOfGameStats.stats.level), 20, 90, 20, BLACK);
    DrawText(TextFormat("SCORE: %u", endOfGameStats.stats.score), 20, 120, 20, BLACK);
    DrawText(TextFormat("CLEARED: %u", endOfGameStats.stats.lines_cleared), 20, 150, 20, BLACK);

}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    // TODO: Unload ENDING screen variables here!
}

// Ending Screen should finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}