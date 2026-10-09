#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include <chrono>
#include <thread>

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 800.0f;
constexpr Vector2 CENTER{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

// Ball can move half the screen width per-second
constexpr float BALL_SPEED = SCREEN_WIDTH * 0.5f;
constexpr float BALL_SIZE = 40.0f;

// Paddles can move half the screen height per-second
constexpr float PADDLE_SPEED = SCREEN_HEIGHT * 0.5f;
constexpr float PADDLE_WIDTH = 40.0f;
constexpr float PADDLE_HEIGHT = 80.0f;

Rectangle RecFromPoint(Vector2 position, float width, float height)
{
    Rectangle rec;
    rec.x = position.x - width * 0.5f;
    rec.y = position.y - height * 0.5f;
    rec.width = width;
    rec.height = height;
    return rec;
}

Rectangle BallRec(Vector2 position)
{
    Rectangle rec = RecFromPoint(position, BALL_SIZE, BALL_SIZE);
    return rec;
}

Rectangle PaddleRec(Vector2 position)
{
    Rectangle rec = RecFromPoint(position, PADDLE_WIDTH, PADDLE_HEIGHT);
    return rec;
}

void ResetBall(Vector2& position, Vector2& direction)
{
    position = CENTER;
    direction = Vector2Rotate(Vector2UnitX, GetRandomValue(0, 360) * DEG2RAD);
}

int main()
{
    Vector2 ball_position;
    Vector2 ball_direction;
    ResetBall(ball_position, ball_direction);

    Vector2 paddle1_position, paddle2_position;
    paddle1_position.x = SCREEN_WIDTH * 0.05f;
    paddle2_position.x = SCREEN_WIDTH * 0.95f;
    paddle1_position.y = paddle2_position.y = CENTER.y;

    int player1_score = 0;
    int player2_score = 0;
    float game_over_timer = 0.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound coin = LoadSound("./assets/audio/sound_coin.mp3");
    Sound thud = LoadSound("./assets/audio/sound_thud.mp3");

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Move paddle with key input
        if (IsKeyDown(KEY_W))
            paddle1_position.y -= PADDLE_SPEED * dt;
        if (IsKeyDown(KEY_S))
            paddle1_position.y += PADDLE_SPEED * dt;

        // Move paddle with key input
        if (IsKeyDown(KEY_UP))
            paddle2_position.y -= PADDLE_SPEED * dt;
        if (IsKeyDown(KEY_DOWN))
            paddle2_position.y += PADDLE_SPEED * dt;


        float phh = PADDLE_HEIGHT * 0.5f;
        paddle1_position.y = Clamp(paddle1_position.y, phh, SCREEN_HEIGHT - phh);
        paddle2_position.y = Clamp(paddle2_position.y, phh, SCREEN_HEIGHT - phh);

        // Change the ball's direction on-collision
        Vector2 ball_position_next = ball_position + ball_direction * BALL_SPEED * dt;
        Rectangle ball_rec = BallRec(ball_position_next);
        Rectangle paddle1_rec = PaddleRec(paddle1_position);
        Rectangle paddle2_rec = PaddleRec(paddle2_position);

        // player1
        
        if (ball_rec.x + ball_rec.width >= SCREEN_WIDTH)
        {
            ball_direction.x *= -1.0f;
            player1_score++;
            PlaySound(coin);
        }

        // player2
        if (ball_rec.x <= 0.0f)
        {
            ball_direction.x *= -1.0f;
            player2_score++;
            PlaySound(coin);
        }

        //Ball bounce
        if (ball_rec.y <= 0.0f || ball_rec.y + ball_rec.height >= SCREEN_HEIGHT)
        {
            ball_direction.y *= -1.0f;
        }

        if (CheckCollisionRecs(ball_rec, paddle2_rec))
        {
            ball_direction.x *= -1.0f;
            player2_score++;
            PlaySound(thud);
        }


        if (CheckCollisionRecs(ball_rec, paddle1_rec))
        {
            ball_direction.x *= -1.0f;
            player1_score++;
            PlaySound(thud);
        }

        // check the players scores. Show the wins
        if (player1_score >= 5 || player2_score >= 5)
        {
            game_over_timer += dt;
            DrawText("Game Over!", SCREEN_WIDTH * 0.5f - MeasureText("Game Over!", 40) * 0.5f, SCREEN_HEIGHT * 0.5f - 20, 40, YELLOW);
            if (player1_score >= 5)
            {
                DrawText("Player 1 Wins!", SCREEN_WIDTH * 0.3f - MeasureText("Player 1 Wins!", 30) * 0.5f, SCREEN_HEIGHT * 0.5f + 20, 30, RED);
            }
            if (player2_score >= 5)
            {
                DrawText("Player 2 Wins!", SCREEN_WIDTH * 0.7f - MeasureText("Player 2 Wins!", 30) * 0.5f, SCREEN_HEIGHT * 0.5f + 20, 30, RED);
            }
            if (game_over_timer >= 3.0f)
            {
                player1_score = 0;
                player2_score = 0;
                game_over_timer = 0.0f;
                ResetBall(ball_position, ball_direction);
            }
        }

        // Update ball position after collision resolution, then render
        ball_position = ball_position + ball_direction * BALL_SPEED * dt;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec(BallRec(ball_position), BLUE);
        DrawRectangleRec(PaddleRec(paddle1_position), PURPLE);
        DrawRectangleRec(PaddleRec(paddle2_position), PURPLE);

        // Text format requires you to put a '%i' wherever you want an integer, then add said integer after the comma
        const char* player1_score_text = TextFormat("Player1 Score: %i ", player1_score);
        const char* player2_score_text = TextFormat("Player2 Score: %i ", player2_score);

        // We can measure our text for more exact positioning. This puts our score in the center of our screen!
        DrawText(player1_score_text, SCREEN_WIDTH * 0.1f - MeasureText(player1_score_text, 20) * 0.5f, 50, 20, GREEN);
        DrawText(player2_score_text, SCREEN_WIDTH * 0.9f - MeasureText(player2_score_text, 20) * 0.5f, 50, 20, GREEN);
        EndDrawing();
    }

    UnloadSound(coin);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
