#include <iostream>
#include "game.h"
#include "menu.h"

// Colors
Color Green = Color{ 38, 185, 154, 255 };
Color Dark_Green = Color{ 20, 160, 133, 255 };
Color Light_Green = Color{ 129, 204, 184, 255 };
Color Yellow = Color{ 243, 213, 91, 255 };

int player_score = 0;
int cpu_score = 0;

// Helper function to reset the ball
void ResetBall(Ball& ball, int screenWidth, int screenHeight) {
    ball.position.x = screenWidth / 2.0f;
    ball.position.y = screenHeight / 2.0f;

    int speed_choices[2] = { -1, 1 };
    ball.velocity.x *= speed_choices[GetRandomValue(0, 1)];
    ball.velocity.y *= speed_choices[GetRandomValue(0, 1)];
}

int main() {
    std::cout << "Starting the game" << std::endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My Pong Game!");
    SetTargetFPS(60);

    // --- Instantiate Objects using the new Constructors ---

    Ball ball(
        Vector2{ screen_width / 2.0f, screen_height / 2.0f }, 
        Yellow, 20.0f, 
        "assets/textures/ball/basic_ball_5.png"
    );
    ball.velocity = Vector2{ 7.0f, 7.0f };

    Paddle player(
        Vector2{ screen_width - 20.0f - 10.0f - 25.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f,
        "assets/textures/paddles/basic_paddle.png"
    );

    CpuPaddle cpu(
        Vector2{ 20.0f + 10.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f,
        Difficulty::Normal,
        "assets/textures/paddles/basic_paddle_2.png"
    );

    // --- Setup Menu and Game State ---

    GameState currentState = GameState::MainMenu;
    float sessionPlayTime = 0.0f;
    bool isPaused = false;
    Menu mainMenu("PONG RELOADED", { "Singleplayer", "Multiplayer", "Settings", "Quit" });
    Menu difficultyMenu("SELECT DIFFICULTY", { "Easy", "Normal", "Hard", "Back" });

    // --- Load Background and Wall Textures ---
    Texture2D courtBackground = LoadTexture("assets/textures/spaces/basic_space.png");
    Texture2D wallsTexture = LoadTexture("assets/textures/spaces/walls.png");
    Texture2D lineTexture = LoadTexture("assets/textures/hud/line.png");

    // --- Main Game Loop ---

    while (WindowShouldClose() == false && currentState != GameState::GameOver) {
        BeginDrawing();
        ClearBackground(Dark_Green);

        switch (currentState) {
        case GameState::MainMenu:
        {
            int selected = mainMenu.Update();
            if (selected == 0) {
                currentState = GameState::DifficultySelect;
            }
            if (selected == 1) {
                currentState = GameState::Multiplayer;
            }
            if (selected == 2) {
                currentState = GameState::Settings;
            }
            else if (selected == 3) {
                currentState = GameState::GameOver;
            }
            mainMenu.Draw();
            break;
        }

        case GameState::DifficultySelect:
        {
            int selected = difficultyMenu.Update();
            if (selected >= 0 && selected <= 2) {
                if (selected == 0) {
                    cpu.SetDifficulty(Difficulty::Easy);
                }
                else if (selected == 1) {
                    cpu.SetDifficulty(Difficulty::Normal);
                }
                else if (selected == 2) {
                    cpu.SetDifficulty(Difficulty::Hard);
                }
                
                // Reset game session stats
                player_score = 0;
                cpu_score = 0;
                sessionPlayTime = 0.0f;
                isPaused = false;
                ResetBall(ball, screen_width, screen_height);
                currentState = GameState::Playing;
            }
            else if (selected == 3) {
                currentState = GameState::MainMenu;
            }
            difficultyMenu.Draw();
            break;
        }

        case GameState::Playing:
        {
            // Toggle pause state with 'P' key
            if (IsKeyPressed(KEY_P)) {
                isPaused = !isPaused;
            }

            if (!isPaused) {
                sessionPlayTime += GetFrameTime();

                ball.Update();
                player.Update();
                cpu.Update(ball.position.y);

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
                    ball.velocity.x *= -1;
                }

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
                    ball.velocity.x *= -1;
                }

                if (ball.position.x + ball.radius >= screen_width - 20.0f) {
                    cpu_score++;
                    ResetBall(ball, screen_width, screen_height);
                }
                if (ball.position.x - ball.radius <= 20.0f) {
                    player_score++;
                    ResetBall(ball, screen_width, screen_height);
                }
            }

            // --- Draw Textured Court Background ---
            // Left Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ 20.0f, 20.0f, 570.0f, 760.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Right Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ 690.0f, 20.0f, 570.0f, 760.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Center strip (Dark Green background area) - already cleared by ClearBackground

            // Center Circle
            DrawCircle(screen_width / 2, screen_height / 2, 50.0f, Color{ 102, 51, 153, 100 });
            DrawCircleLines(screen_width / 2, screen_height / 2, 50.0f, Color{ 50, 25, 75, 250 });

            // Tiled Center Dashed Line (line.png)
            int lineY = 20;
            while (lineY < 780) {
                DrawTexture(lineTexture, screen_width / 2 - lineTexture.width / 2, lineY, WHITE);
                lineY += lineTexture.height;
            }

            // Top Wall (walls.png)
            DrawTexturePro(
                wallsTexture,
                Rectangle{ 0.0f, 0.0f, (float)wallsTexture.width, (float)wallsTexture.height },
                Rectangle{ 0.0f, 0.0f, (float)screen_width, 20.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Bottom Wall (walls.png)
            DrawTexturePro(
                wallsTexture,
                Rectangle{ 0.0f, 0.0f, (float)wallsTexture.width, (float)wallsTexture.height },
                Rectangle{ 0.0f, (float)screen_height - 20.0f, (float)screen_width, 20.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Left Wall (walls.png)
            DrawTexturePro(
                wallsTexture,
                Rectangle{ 0.0f, 0.0f, (float)wallsTexture.width, (float)wallsTexture.height },
                Rectangle{ 0.0f, 0.0f, 20.0f, (float)screen_height },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Right Wall (walls.png)
            DrawTexturePro(
                wallsTexture,
                Rectangle{ 0.0f, 0.0f, (float)wallsTexture.width, (float)wallsTexture.height },
                Rectangle{ (float)screen_width - 20.0f, 0.0f, 20.0f, (float)screen_height },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            // Draw Playtime Counter
            int minutes = (int)sessionPlayTime / 60;
            int seconds = (int)sessionPlayTime % 60;
            int timeTextWidth = MeasureText(TextFormat("%02i:%02i", minutes, seconds), 32);
            
            // Draw background box to block out the center line and increase contrast
            DrawRectangle(screen_width / 2 - timeTextWidth / 2 - 15, 715, timeTextWidth + 30, 44, Color{ 15, 15, 15, 220 });
            DrawRectangleLines(screen_width / 2 - timeTextWidth / 2 - 15, 715, timeTextWidth + 30, 44, Color{ 100, 100, 100, 255 });
            DrawText(TextFormat("%02i:%02i", minutes, seconds), screen_width / 2 - timeTextWidth / 2, 721, 32, YELLOW);

            ball.Draw();
            cpu.Draw();
            player.Draw();

            // Draw Pause Overlay and Text
            if (isPaused) {
                // Semi-transparent overlay inside the borders
                DrawRectangle(20, 20, screen_width - 40, screen_height - 40, Color{ 0, 0, 0, 150 });
                
                int pausedTextWidth = MeasureText("PAUSED", 60);
                DrawText("PAUSED", screen_width / 2 - pausedTextWidth / 2, screen_height / 2 - 30, 60, YELLOW);
            }

            break;
        }
        default:
            break;
        }

        EndDrawing();
    }

    UnloadTexture(courtBackground);
    UnloadTexture(wallsTexture);
    UnloadTexture(lineTexture);

    CloseWindow();
    return 0;
}