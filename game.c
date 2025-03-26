#include <iso646.h>

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#define DEV_MODE

#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 150
#define BALL_SIZE 18
#define BALL_SPEED 400.0f
#define WIN_SCORE 10

typedef struct Paddle {
    Rectangle rect;
    float speed;
} Paddle;

typedef struct Ball {
    Vector2 position;
    Vector2 direction;
    float speed;
} Ball;

typedef struct {
    Sound hit;
    Sound edge;
    Sound top;
} Sounds;

typedef enum {
    MAIN_MENU,
    PREGAME,
    GAME,
    GAME_OVER
} Scene;

typedef struct {
    int leftScore;
    int rightScore;
    bool isPaused;
    Scene currentScene;
    Sounds sounds;
} GameState;

const int screenWidth = 1280;
const int screenHeight = 720;

void GameLogic(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, GameState *state);
void ResetBall(Ball *ball);
void DrawDashedLine(Color color);
void DrawMainMenu();
void DrawGame();
void DrawGameOver();

// Function to reset the ball
void ResetBall(Ball *ball) {
    ball->position = (Vector2){(int)(screenWidth / 2),(int)( screenHeight / 2)};
    ball->direction.x = (GetRandomValue(0, 1) == 0) ? 1.0f : -1.0f; // Randomize initial direction
    ball->direction.y = (GetRandomValue(-1, 1) < 0) ? 1.0f : -1.0f; // Randomize initial vertical direction
    float length = sqrt(ball->direction.x * ball->direction.x + ball->direction.y * ball->direction.y);
    ball->direction.x /= length;
    ball->direction.y /= length;
    ball->speed = BALL_SPEED;
}

int main(void) {
    SearchAndSetResourceDir("resources");

    SetTraceLogLevel( LOG_ALL );
    SetConfigFlags( FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Pong Game");
    SetTargetFPS(60);

    InitAudioDevice();
    Sound sn_beep = LoadSound("sounds/ping_pong_8bit_beeep.ogg");
    Sound sn_peep = LoadSound("sounds/ping_pong_8bit_peeeeeep.ogg");
    Sound sn_plop = LoadSound("sounds/ping_pong_8bit_plop.ogg");

    GameState state = {
        .leftScore = 0,
        .rightScore = 0,
        .isPaused = false,
        .currentScene = MAIN_MENU,
        .sounds = {.top = sn_beep, .edge = sn_peep, .hit = sn_plop},
    };

#ifdef DEV_MODE
    state.currentScene = PREGAME;
#endif

    Paddle leftPaddle = {{50, (int)((screenHeight / 2)) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT}, 500.0f};
    Paddle rightPaddle = {{screenWidth - 50 - PADDLE_WIDTH, (int)((screenHeight / 2)) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT}, 500.0f};
    Ball ball = {(Vector2){(int)(screenWidth / 2),(int)( screenHeight / 2)}, (Vector2){1.0f, 1.0f}, BALL_SPEED}; // Direction normalized

    // Normalize the direction vector
    float length = sqrt(ball.direction.x * ball.direction.x + ball.direction.y * ball.direction.y);
    ball.direction.x /= length;
    ball.direction.y /= length;

    ResetBall(&ball); // Initial reset
    while (!WindowShouldClose()) {
        switch (state.currentScene) {
            case MAIN_MENU:
                if (IsKeyPressed(KEY_ONE)) {
                    state.currentScene = GAME;
                }
                else if (IsKeyPressed(KEY_TWO)) {
                }
                else if (IsKeyPressed(KEY_THREE)) {
                    CloseWindow();
                }
                break;
            case PREGAME:
                if (IsKeyPressed(KEY_SPACE) or IsKeyPressed(KEY_ENTER))
                    state.currentScene = GAME;
                break;
            case GAME:
                if (!state.isPaused)
                    GameLogic(&leftPaddle, &rightPaddle, &ball, &state);
                if (IsKeyPressed(KEY_P)) {
                    state.isPaused = !state.isPaused;
                }
                break;
            case GAME_OVER:
                ball.speed = 0.0f;
                if (IsKeyPressed(KEY_R)) {
                    state.leftScore = 0;
                    state.rightScore = 0;
                    ResetBall(&ball);
                    state.currentScene = GAME;
                }
                else if (IsKeyPressed(KEY_M)) {
                    state.currentScene = MAIN_MENU;
                }
                break;
            default: break;
        }

        BeginDrawing();
        switch (state.currentScene) {
            case MAIN_MENU:
                ClearBackground(DARKGRAY);
                DrawText("Main Menu", GetScreenWidth() / 2 - MeasureText("Main Menu", 40) / 2, GetScreenHeight() / 2 - 80, 40, RAYWHITE);
                DrawText("1. Play Game", GetScreenWidth() / 2 - MeasureText("1. Play Game", 20) / 2, GetScreenHeight() / 2, 20, RAYWHITE);
                DrawText("2. Settings", GetScreenWidth() / 2 - MeasureText("2. Settings", 20) / 2, GetScreenHeight() / 2 + 40, 20, RAYWHITE);
                DrawText("3. Exit", GetScreenWidth() / 2 - MeasureText("3. Exit", 20) / 2, GetScreenHeight() / 2 + 80, 20, RAYWHITE);
                break;
            case PREGAME:
                ClearBackground(DARKGRAY);
                DrawRectangleRec(leftPaddle.rect, LIGHTGRAY);
                DrawRectangleRec(rightPaddle.rect, LIGHTGRAY);
                DrawDashedLine(LIGHTGRAY);
                DrawText(TextFormat("%d", state.leftScore), screenWidth / 4, 20, 80, LIGHTGRAY);
                DrawText(TextFormat("%d", state.rightScore), 3 * screenWidth / 4, 20, 80, LIGHTGRAY);
                DrawText(TextFormat("Press to start game."), (screenWidth / 2) - (screenWidth / 4), screenHeight / 3, 60, RAYWHITE);
                break;
            case GAME:
                ClearBackground(DARKGRAY);
                DrawRectangleRec(leftPaddle.rect, RAYWHITE);
                DrawRectangleRec(rightPaddle.rect, RAYWHITE);
                DrawCircleV(ball.position, BALL_SIZE / 2, RAYWHITE);
                DrawDashedLine(RAYWHITE);
                DrawText(TextFormat("%d", state.leftScore), screenWidth / 4, 20, 80, RAYWHITE);
                DrawText(TextFormat("%d", state.rightScore), 3 * screenWidth / 4, 20, 80, RAYWHITE);
                if (state.isPaused) {
                    ClearBackground(DARKGRAY);
                    DrawRectangleRec(leftPaddle.rect, LIGHTGRAY);
                    DrawRectangleRec(rightPaddle.rect, LIGHTGRAY);
                    DrawDashedLine(LIGHTGRAY);
                    DrawText(TextFormat("Paused."), GetScreenWidth() / 2 - MeasureText("Paused", 60) / 2, screenHeight / 2, 60, RED);
                    break;
                }
                break;
            case GAME_OVER:
                ClearBackground(DARKGRAY);
                DrawRectangleRec(leftPaddle.rect, LIGHTGRAY);
                DrawRectangleRec(rightPaddle.rect, LIGHTGRAY);
                DrawDashedLine(LIGHTGRAY);
                DrawText(TextFormat("%d", state.leftScore), screenWidth / 4, 20, 80, LIGHTGRAY);
                DrawText(TextFormat("%d", state.rightScore), 3 * screenWidth / 4, 20, 80, LIGHTGRAY);
                DrawText(TextFormat("Press R to restart game."), (screenWidth / 2) - (screenWidth / 4), screenHeight / 2, 60, RAYWHITE);
                break;
            }
        EndDrawing();
    }

    // De-Initialization
    UnloadSound(sn_beep);
    UnloadSound(sn_peep);
    UnloadSound(sn_plop);
    CloseAudioDevice();     // Close audio device
    CloseWindow(); // Close window and OpenGL context

    return 0;
}

void GameLogic(Paddle *leftPaddle, Paddle *rightPaddle, Ball *ball, GameState *state) {
    // Input
    if (IsKeyDown(KEY_W) && leftPaddle->rect.y > 0) {
        leftPaddle->rect.y -= leftPaddle->speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_S) && leftPaddle->rect.y < screenHeight - PADDLE_HEIGHT) {
        leftPaddle->rect.y += leftPaddle->speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_UP) && rightPaddle->rect.y > 0) {
        rightPaddle->rect.y -= rightPaddle->speed * GetFrameTime();
    }
    if (IsKeyDown(KEY_DOWN) && rightPaddle->rect.y < screenHeight - PADDLE_HEIGHT) {
        rightPaddle->rect.y += rightPaddle->speed * GetFrameTime();
    }

    // Update ball position
    ball->position.x += ball->direction.x * ball->speed * GetFrameTime();
    ball->position.y += ball->direction.y * ball->speed * GetFrameTime();

    // Ball collision with top and bottom
    if (ball->position.y <= 0 || ball->position.y >= screenHeight - BALL_SIZE) {
        ball->direction.y *= -1; // Reverse Y direction
        PlaySound(state->sounds.top);
    }

    // Ball collision with paddles
    if (CheckCollisionCircleRec(ball->position, BALL_SIZE / 2, leftPaddle->rect) || 
        CheckCollisionCircleRec(ball->position, BALL_SIZE / 2, rightPaddle->rect)) {
        ball->direction.x *= -1; // Reverse X direction
        ball->speed += BALL_SPEED / 10.0;
        PlaySound(state->sounds.hit);
    }

    // Scoring
    if (ball->position.x < 0) {
        PlaySound(state->sounds.edge);
        state->rightScore++; // Right player scores
        ResetBall(ball);
    }
    if (ball->position.x > screenWidth) {
        PlaySound(state->sounds.edge);
        state->leftScore++;  // Left player scores
        ResetBall(ball);
    }

    if (state->leftScore == WIN_SCORE or state->rightScore == WIN_SCORE)
        state->currentScene = GAME_OVER;
}

void DrawDashedLine(Color color) {
    Vector2 start = { (int)(screenWidth / 2), 0 };
    Vector2 end = { (int)(screenWidth / 2), screenHeight };
    float dashLength = 10.0f;
    float gapLength = 5.0f;
    Vector2 direction = Vector2Subtract(end, start);
    float length = Vector2Length(direction);
    direction = Vector2Normalize(direction);

    float totalLength = 0.0f;
    while (totalLength < length) {
        // Calculate the start and end points of the dash
        Vector2 dashStart = Vector2Add(start, Vector2Scale(direction, totalLength));
        Vector2 dashEnd = Vector2Add(dashStart, Vector2Scale(direction, dashLength));

        // Draw the dash
        DrawLineEx(dashStart, dashEnd, 5, color);

        // Move the total length by the dash length and gap length
        totalLength += dashLength + gapLength;
    }
}
