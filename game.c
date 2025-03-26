#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 150
#define BALL_SIZE 18
#define BALL_SPEED 400.0f

typedef struct Paddle {
    Rectangle rect;
    float speed;
} Paddle;

typedef struct Ball {
    Vector2 position;
    Vector2 direction; // Direction vector
    float speed;       // Speed scalar
} Ball;

const int screenWidth = 1280;
const int screenHeight = 720;

void ResetBall(Ball *ball);
void DrawDashedLine();

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
    SetTraceLogLevel( LOG_ALL );
    SetConfigFlags( FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Pong Game");
    SetTargetFPS(60);

    SearchAndSetResourceDir("resources");

    InitAudioDevice();      // Initialize audio device

    Sound sn_beep = LoadSound("sounds/ping_pong_8bit_beeep.ogg");
    Sound sn_peep = LoadSound("sounds/ping_pong_8bit_peeeeeep.ogg");
    Sound sn_plop = LoadSound("sounds/ping_pong_8bit_plop.ogg");

    Paddle leftPaddle = {{50, (int)((screenHeight / 2)) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT}, 500.0f};
    Paddle rightPaddle = {{screenWidth - 50 - PADDLE_WIDTH, (int)((screenHeight / 2)) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT}, 500.0f};
    Ball ball = {(Vector2){(int)(screenWidth / 2),(int)( screenHeight / 2)}, (Vector2){1.0f, 1.0f}, BALL_SPEED}; // Direction normalized

    // Normalize the direction vector
    float length = sqrt(ball.direction.x * ball.direction.x + ball.direction.y * ball.direction.y);
    ball.direction.x /= length;
    ball.direction.y /= length;

    // Score variables
    int leftScore = 0;
    int rightScore = 0;

    ResetBall(&ball); // Initial reset

    while (!WindowShouldClose()) {
        // Update
        if (IsKeyDown(KEY_W) && leftPaddle.rect.y > 0) {
            leftPaddle.rect.y -= leftPaddle.speed * GetFrameTime();
        }
        if (IsKeyDown(KEY_S) && leftPaddle.rect.y < screenHeight - PADDLE_HEIGHT) {
            leftPaddle.rect.y += leftPaddle.speed * GetFrameTime();
        }
        if (IsKeyDown(KEY_UP) && rightPaddle.rect.y > 0) {
            rightPaddle.rect.y -= rightPaddle.speed * GetFrameTime();
        }
        if (IsKeyDown(KEY_DOWN) && rightPaddle.rect.y < screenHeight - PADDLE_HEIGHT) {
            rightPaddle.rect.y += rightPaddle.speed * GetFrameTime();
        }

        // Update ball position
        ball.position.x += ball.direction.x * ball.speed * GetFrameTime();
        ball.position.y += ball.direction.y * ball.speed * GetFrameTime();

        // Ball collision with top and bottom
        if (ball.position.y <= 0 || ball.position.y >= screenHeight - BALL_SIZE) {
            ball.direction.y *= -1; // Reverse Y direction
            PlaySound(sn_beep);
        }

        // Ball collision with paddles
        if (CheckCollisionCircleRec(ball.position, BALL_SIZE / 2, leftPaddle.rect) || 
            CheckCollisionCircleRec(ball.position, BALL_SIZE / 2, rightPaddle.rect)) {
            ball.direction.x *= -1; // Reverse X direction
            ball.speed += BALL_SPEED / 10.0;
            PlaySound(sn_plop);
        }

        // Scoring
        if (ball.position.x < 0) {
            PlaySound(sn_peep);
            rightScore++; // Right player scores
            ResetBall(&ball);  // Reset ball after scoring
        }
        if (ball.position.x > screenWidth) {
            PlaySound(sn_peep);
            leftScore++;  // Left player scores
            ResetBall(&ball);  // Reset ball after scoring
        }

        // Draw
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawRectangleRec(leftPaddle.rect, RAYWHITE);
        DrawRectangleRec(rightPaddle.rect, RAYWHITE);
        DrawCircleV(ball.position, BALL_SIZE / 2, RAYWHITE);
        DrawDashedLine();

        // Draw scores
        DrawText(TextFormat("%d", leftScore), screenWidth / 4, 20, 80, RAYWHITE);
        DrawText(TextFormat("%d", rightScore), 3 * screenWidth / 4, 20, 80, RAYWHITE);

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

void DrawDashedLine() {
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
        DrawLineEx(dashStart, dashEnd, 5, RAYWHITE);

        // Move the total length by the dash length and gap length
        totalLength += dashLength + gapLength;
    }
}
