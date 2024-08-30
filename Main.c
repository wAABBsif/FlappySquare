#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

typedef struct Square
{
    float positionY;
    float velocityY;
} Square;

static void Game();
static void Square_Update();
static void Pipe_Update();
static void SavePlayerData();
static void LoadPlayerData();

static Vector2 s_pipes[2];
static short s_highScore;
static short s_score;
static Square s_square;
static bool s_isGameEnd;

int main(void)
{
    s_highScore = 0;
    SetRandomSeed(time(NULL));
    LoadPlayerData();
    InitWindow(800, 450, "Flappy Square");
    Game();
    CloseWindow();

    return 0;
}

static void Game()
{
    for (char i = 0; i < sizeof(s_pipes) / sizeof(Vector2); i++)
        s_pipes[i] = (Vector2){1500 - i * 450, GetRandomValue(0, 325)};

    s_score = 0;
    s_square = (Square){225, 0};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        Square_Update();
        Pipe_Update();
        if (s_isGameEnd)
        {
            s_highScore = s_score > s_highScore ? s_score : s_highScore;
            const char* scoreText = TextFormat("Score: %i", s_score);
            DrawText(scoreText, (800 - MeasureText(scoreText, 50)) / 2, 160, 50, WHITE);
            const char* highscoreText = TextFormat("Highscore: %i", s_highScore);
            DrawText(highscoreText, (800 - MeasureText(highscoreText, 50)) / 2, 224, 50, WHITE);
            DrawText("Press space to restart!", (800 - MeasureText("Press space to restart", 50)) / 2, 288, 50, WHITE);
            if (IsKeyPressed(KEY_SPACE))
            {
                s_isGameEnd = false;
                return Game();
            }
        }
        else
        {
            DrawText(TextFormat("%i", s_score), 8, 0, 80, BLACK);
        }
        static bool s_drawFPS;
        if (IsKeyPressed(KEY_F))
            s_drawFPS = !s_drawFPS;
        DrawFPS(s_drawFPS ? 64 : -255, 0);
        EndDrawing();
    }
}

static void Square_Update()
{
    if (IsKeyPressed(KEY_SPACE))
        s_square.velocityY = -265;
    s_square.velocityY = Clamp(s_square.velocityY + 540.0f * GetFrameTime(), -800, 540);
    if (!s_isGameEnd)
        s_square.positionY += s_square.velocityY * GetFrameTime();
    DrawRectangle(100, s_square.positionY, 50, 50, RAYWHITE);
}

static void Pipe_Update()
{
    for (char i = 0; i < sizeof(s_pipes) / sizeof(Vector2); i++)
    {
        if (!s_isGameEnd)
        {
            if ((s_pipes[i].x -= GetFrameTime() * 220) < -150)
            {
                s_score++;
                s_pipes[i].y = GetRandomValue(0, 325);
                s_pipes[i].x += 950;
            }
        }

        DrawRectangle((int)s_pipes[i].x, 0, 150, s_pipes[i].y, DARKGRAY);
        DrawRectangle((int)s_pipes[i].x, s_pipes[i].y + 125, 150, 450, DARKGRAY);

        if (s_pipes[i].x > -50 && s_pipes[i].x < 150 && !s_isGameEnd)
        {
            if (s_pipes[i].y > s_square.positionY || s_pipes[i].y + 130 < s_square.positionY + 45)
            {
                SavePlayerData();
                s_isGameEnd = true;
            }
        }
    }
}

static void SavePlayerData()
{
    FILE* stream = fopen("FlappySquare.sav", "w");
    if (stream == NULL)
        return;

    fwrite(&s_highScore, sizeof(s_highScore), 1, stream);
    fclose(stream);
}

static void LoadPlayerData()
{
    FILE* stream = fopen("FlappySquare.sav", "r");
    if (stream == NULL)
        return;

    fread(&s_highScore, sizeof(s_highScore), 1, stream);
    fclose(stream);
}