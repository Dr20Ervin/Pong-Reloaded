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

    Ball ball(Vector2{ screen_width / 2.0f, screen_height / 2.0f }, Yellow, 20.0f);
    ball.velocity = Vector2{ 7.0f, 7.0f };

    Paddle player(
        Vector2{ screen_width - 35.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f
    );

    CpuPaddle cpu(
        Vector2{ 10.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f,
        Difficulty::Normal
        );

    // --- Setup Menu and Game State ---

    GameState currentState = GameState::MainMenu;
    Menu mainMenu("PONG RELOADED", { "Start Game", "Quit" });
    Menu difficultyMenu("SELECT DIFFICULTY", { "Easy", "Normal", "Hard", "Back" });

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
            else if (selected == 1) {
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
            ball.Update();
            player.Update();
            cpu.Update(ball.position.y);

            if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
                ball.velocity.x *= -1;
            }

            if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
                ball.velocity.x *= -1;
            }

            if (ball.position.x + ball.radius >= screen_width) {
                cpu_score++;
                ResetBall(ball, screen_width, screen_height);
            }
            if (ball.position.x - ball.radius <= 0) {
                player_score++;
                ResetBall(ball, screen_width, screen_height);
            }

            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);

            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            ball.Draw();
            cpu.Draw();
            player.Draw();

            break;
        }
        default:
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}