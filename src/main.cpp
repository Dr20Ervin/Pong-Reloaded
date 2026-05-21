#include <iostream>
#include "game.h"
#include "menu.h"

// Global colors
Color Green = Color{ 38, 185, 154, 255 };
Color Dark_Green = Color{ 20, 160, 133, 255 };
Color Light_Green = Color{ 129, 204, 184, 255 };
Color Yellow = Color{ 243, 213, 91, 255 };

int main() {
    std::cout << "Starting game session" << std::endl;
    int screen_width = 1280;
    int screen_height = 800;
    
    InitWindow(screen_width, screen_height, "Pong Reloaded");
    SetTargetFPS(60);
    InitAudioDevice();

    // App state context
    GameContext ctx;

    // Asset textures loading
    ctx.courtBackground = LoadTexture("assets/textures/spaces/basic_space.png");
    ctx.wallsTexture = LoadTexture("assets/textures/spaces/walls.png");
    ctx.lineTexture = LoadTexture("assets/textures/hud/line.png");

    // Audio assets loading
    ctx.paddleHitSound = LoadSound("assets/audio/paddle_hit.ogg");
    ctx.wallHitSound = LoadSound("assets/audio/wall_hit.ogg");
    ctx.scoreSound = LoadSound("assets/audio/score.ogg");

    // Entities instantiation
    Ball ball(
        Vector2{ screen_width / 2.0f, screen_height / 2.0f }, 
        Yellow, 20.0f, 
        "assets/textures/ball/basic_ball_5.png"
    );
    ResetBall(ball);

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

    // Menu instantiation
    Menu mainMenu("PONG RELOADED", { "Singleplayer", "Multiplayer", "Settings", "Quit" });
    Menu difficultyMenu("SELECT DIFFICULTY", { "Easy", "Normal", "Hard", "Back" });

    // Main loop
    while (WindowShouldClose() == false && !ctx.shouldQuit) {
        screen_width = GetScreenWidth();
        screen_height = GetScreenHeight();

        // Update loop
        switch (ctx.currentState) {
        case GameState::MainMenu:
        {
            int selected = mainMenu.Update();
            if (selected == 0) {
                ctx.isMultiplayer = false;
                ctx.currentState = GameState::DifficultySelect;
            }
            else if (selected == 1) {
                ctx.isMultiplayer = true;
                ctx.score.player_score = 0;
                ctx.score.player2_score = 0;
                ctx.score.cpu_score = 0;
                ctx.sessionPlayTime = 0.0f;
                ctx.isPaused = false;
                ctx.p1Ready = false;
                ctx.p2Ready = false;
                ResetBall(ball);
                ctx.currentState = GameState::MultiplayerLobby;
            }
            else if (selected == 2) {
                ctx.currentState = GameState::Settings;
            }
            else if (selected == 3) {
                ctx.shouldQuit = true;
            }
            break;
        }
        case GameState::DifficultySelect:
        {
            int selected = difficultyMenu.Update();
            if (selected >= 0 && selected <= 2) {
                if (selected == 0) cpu.SetDifficulty(Difficulty::Easy);
                else if (selected == 1) cpu.SetDifficulty(Difficulty::Normal);
                else if (selected == 2) cpu.SetDifficulty(Difficulty::Hard);

                ctx.score.player_score = 0;
                ctx.score.player2_score = 0;
                ctx.score.cpu_score = 0;
                ctx.sessionPlayTime = 0.0f;
                ctx.isPaused = false;
                ResetBall(ball);
                ctx.currentState = GameState::Playing;
            }
            else if (selected == 3) {
                ctx.currentState = GameState::MainMenu;
            }
            break;
        }
        case GameState::MultiplayerLobby:
            UpdateLobbyState(ctx);
            break;
        case GameState::Multiplayer:
            UpdateMultiplayerState(ctx, ball, player, cpu);
            break;
        case GameState::Settings:
            UpdateSettingsState(ctx);
            break;
        case GameState::Playing:
            UpdatePlayingState(ctx, ball, player, cpu);
            break;
        case GameState::GameOver:
            UpdateGameOverState(ctx);
            break;
        default:
            break;
        }

        // Draw loop
        BeginDrawing();
        ClearBackground(Dark_Green);

        switch (ctx.currentState) {
        case GameState::MainMenu:
        {
            mainMenu.Draw();
            int menuHintWidth = MeasureText("Use UP/DOWN to navigate | ENTER to select", 20);
            DrawText("Use UP/DOWN to navigate | ENTER to select",
                screen_width / 2 - menuHintWidth / 2,
                screen_height - 80, 20, WHITE);
            break;
        }
        case GameState::DifficultySelect:
        {
            difficultyMenu.Draw();
            int gameHintWidth = MeasureText("Player: UP/DOWN Arrows to move | P to Pause | SPACE to Force Game Over", 20);
            DrawText("Player: UP/DOWN Arrows to move | P to Pause | SPACE to Force Game Over",
                screen_width / 2 - gameHintWidth / 2,
                screen_height - 50, 20, WHITE);
            break;
        }
        case GameState::MultiplayerLobby:
            DrawLobbyState(ctx, screen_width, screen_height);
            break;
        case GameState::Multiplayer:
            DrawMultiplayerState(ctx, ball, player, cpu, screen_width, screen_height);
            break;
        case GameState::Settings:
            DrawSettingsState(ctx, screen_width, screen_height);
            break;
        case GameState::Playing:
            DrawPlayingState(ctx, ball, player, cpu, screen_width, screen_height);
            break;
        case GameState::GameOver:
            DrawGameOverState(ctx, screen_width, screen_height);
            break;
        default:
            break;
        }

        EndDrawing();
    }

    // Asset unloading and cleanup
    UnloadTexture(ctx.courtBackground);
    UnloadTexture(ctx.wallsTexture);
    UnloadTexture(ctx.lineTexture);
    UnloadSound(ctx.paddleHitSound);
    UnloadSound(ctx.wallHitSound);
    UnloadSound(ctx.scoreSound);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}