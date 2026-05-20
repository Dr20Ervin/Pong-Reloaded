#include <iostream>
#include "game.h"
#include "menu.h"

// Colors
Color Green = Color{ 38, 185, 154, 255 };
Color Dark_Green = Color{ 20, 160, 133, 255 };
Color Light_Green = Color{ 129, 204, 184, 255 };
Color Yellow = Color{ 243, 213, 91, 255 };

struct ScoreBoard
{
    int player_score = 0;
    int player2_score = 0;
    int cpu_score = 0;
};


void ApplyResolution(int width, int height) {
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    
    if (width >= monitorWidth || height >= monitorHeight) {
        SetWindowState(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    }
    else {
        ClearWindowState(FLAG_WINDOW_MAXIMIZED);
        SetWindowSize(width, height);
        // Center the window on the active monitor
        SetWindowPosition(monitorWidth / 2 - width / 2, monitorHeight / 2 - height / 2);
    }
}

void ApplyFramerate(int option) {
    if (option == 0) {
        ClearWindowState(FLAG_VSYNC_HINT);
        SetTargetFPS(60);
    }
    else if (option == 1) {
        ClearWindowState(FLAG_VSYNC_HINT);
        SetTargetFPS(144);
    }
    else if (option == 2) {
        SetWindowState(FLAG_VSYNC_HINT);
        SetTargetFPS(0);
    }
}

// Helper function to reset the ball
void ResetBall(Ball& ball) {
    ball.position.x = GetScreenWidth() / 2.0f;
    ball.position.y = GetScreenHeight() / 2.0f;

    int speed_choices[2] = { -1, 1 };
    ball.velocity.x = BALL_SPEED * speed_choices[GetRandomValue(0, 1)];
    ball.velocity.y = BALL_SPEED * speed_choices[GetRandomValue(0, 1)];
}

int main() {
    std::cout << "Starting the game" << std::endl;
    int screen_width = 1280;
    int screen_height = 800;
    InitWindow(screen_width, screen_height, "Pong Reloaded");
    SetTargetFPS(60);
    InitAudioDevice();

    // --- Instantiate Objects using the new Constructors ---

    Ball ball(
        Vector2{ screen_width / 2.0f, screen_height / 2.0f }, 
        Yellow, 20.0f, 
        "assets/textures/ball/basic_ball_5.png"
    );
    ball.velocity = Vector2{ BALL_SPEED, BALL_SPEED };

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
    bool shouldQuit = false;
    bool isMultiplayer = false;
    bool p1Ready = false;
    bool p2Ready = false;
    
    int resolutionOption = 0;      // 0 = 1280x800, 1 = 1600x900, 2 = 1920x1080
    int framerateOption = 0;       // 0 = 60 FPS, 1 = 144 FPS, 2 = VSync
    bool isFullscreen = false;
    int maxScoreOption = 0;        // 0 = 5, 1 = 11, 2 = 15, 3 = 21
    int maxScore = 5;
    bool sfxEnabled = true;
    int selectedSettingLine = 0;   // 0 = Resolution, 1 = Framerate, 2 = Screen Mode, 3 = Score Limit, 4 = Sound, 5 = Back

    Menu mainMenu("PONG RELOADED", { "Singleplayer", "Multiplayer", "Settings", "Quit" });
    Menu difficultyMenu("SELECT DIFFICULTY", { "Easy", "Normal", "Hard", "Back" });
    ScoreBoard score;

    // --- Load Background and Wall Textures ---
    Texture2D courtBackground = LoadTexture("assets/textures/spaces/basic_space.png");
    Texture2D wallsTexture = LoadTexture("assets/textures/spaces/walls.png");
    Texture2D lineTexture = LoadTexture("assets/textures/hud/line.png");

    // --- Load Sound Effects ---
    Sound paddleHitSound = LoadSound("assets/audio/paddle_hit.ogg");
    Sound wallHitSound = LoadSound("assets/audio/wall_hit.ogg");
    Sound scoreSound = LoadSound("assets/audio/score.ogg");

    // --- Main Game Loop ---

    while (WindowShouldClose() == false && !shouldQuit) {
        screen_width = GetScreenWidth();
        screen_height = GetScreenHeight();

        BeginDrawing();
        ClearBackground(Dark_Green);

        switch (currentState) {
        case GameState::MainMenu:
        {
            int selected = mainMenu.Update();
            if (selected == 0) {
                isMultiplayer = false;
                currentState = GameState::DifficultySelect;
            }
            else if (selected == 1) {
                isMultiplayer = true;
                score.player_score = 0;
                score.player2_score = 0;
                score.cpu_score = 0;
                sessionPlayTime = 0.0f;
                isPaused = false;
                p1Ready = false;
                p2Ready = false;
                ResetBall(ball);
                currentState = GameState::MultiplayerLobby;
            }
            else if (selected == 2) {
                currentState = GameState::Settings;
            }
            else if (selected == 3) {
                shouldQuit = true;
            }
            mainMenu.Draw();

            // Controls helper
            int menuHintWidth = MeasureText("Use UP/DOWN to navigate | ENTER to select", 20);
            DrawText("Use UP/DOWN to navigate | ENTER to select",
                screen_width / 2 - menuHintWidth / 2,
                screen_height - 80, 20, WHITE);
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
                score.player_score = 0;
                score.player2_score = 0;
                score.cpu_score = 0;
                sessionPlayTime = 0.0f;
                isPaused = false;
                ResetBall(ball);
                currentState = GameState::Playing;
            }
            else if (selected == 3) {
                currentState = GameState::MainMenu;
            }
            difficultyMenu.Draw();

            // Controls helper
            int gameHintWidth = MeasureText("Game: UP/DOWN Arrows to move | P to Pause | SPACE to Force Game Over", 20);
            DrawText("Player: UP/DOWN Arrows to move | P to Pause | SPACE to Force Game Over",
                screen_width / 2 - gameHintWidth / 2,
                screen_height - 50, 20, WHITE);

            break;
        }

        case GameState::MultiplayerLobby:
        {
            // Input checks
            if (IsKeyPressed(KEY_W)) {
                p1Ready = !p1Ready;
                if (sfxEnabled) PlaySound(paddleHitSound);
            }
            if (IsKeyPressed(KEY_UP)) {
                p2Ready = !p2Ready;
                if (sfxEnabled) PlaySound(paddleHitSound);
            }
            if (IsKeyPressed(KEY_B)) {
                currentState = GameState::MainMenu;
            }

            if (p1Ready && p2Ready) {
                currentState = GameState::Multiplayer;
            }

            // --- Draw Court Background Behind Panel ---
            // Left Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ 20.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Right Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ (float)screen_width / 2.0f + 50.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Center Circle
            DrawCircle(screen_width / 2, screen_height / 2, 50.0f, Color{ 102, 51, 153, 100 });
            DrawCircleLines(screen_width / 2, screen_height / 2, 50.0f, Color{ 50, 25, 75, 250 });

            // Tiled Center Dashed Line (line.png)
            int lineY = 20;
            while (lineY < screen_height - 20) {
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

            // Draw Lobby Panel
            int panelWidth = 800;
            int panelHeight = 450;
            int panelX = screen_width / 2 - panelWidth / 2;
            int panelY = screen_height / 2 - panelHeight / 2;

            // Semi-transparent background panel
            DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{ 15, 15, 15, 230 });
            DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{ 120, 120, 120, 255 });

            // Title
            int titleWidth = MeasureText("MULTIPLAYER LOBBY", 40);
            DrawText("MULTIPLAYER LOBBY", screen_width / 2 - titleWidth / 2, panelY + 30, 40, WHITE);

            // Player 1 Details (Left Paddle controlled by W/S keys)
            int p1X = panelX + 50;
            int p1Y = panelY + 110;
            DrawText("PLAYER 1 (LEFT)", p1X, p1Y, 24, LIGHTGRAY);
            DrawText("Controls: W / S", p1X, p1Y + 45, 20, WHITE);
            DrawText("Press 'W' to toggle ready", p1X, p1Y + 75, 18, GRAY);
            
            if (p1Ready) {
                DrawRectangle(p1X, p1Y + 115, 220, 50, GREEN);
                DrawText("READY", p1X + 110 - MeasureText("READY", 20)/2, p1Y + 130, 20, BLACK);
            } else {
                DrawRectangle(p1X, p1Y + 115, 220, 50, RED);
                DrawText("NOT READY", p1X + 110 - MeasureText("NOT READY", 20)/2, p1Y + 130, 20, WHITE);
            }

            // Player 2 Details (Right Paddle controlled by UP/DOWN arrow keys)
            int p2X = panelX + panelWidth - 270;
            int p2Y = panelY + 110;
            DrawText("PLAYER 2 (RIGHT)", p2X, p2Y, 24, LIGHTGRAY);
            DrawText("Controls: UP / DOWN", p2X, p2Y + 45, 20, WHITE);
            DrawText("Press 'UP' to toggle ready", p2X, p2Y + 75, 18, GRAY);
            
            if (p2Ready) {
                DrawRectangle(p2X, p2Y + 115, 220, 50, GREEN);
                DrawText("READY", p2X + 110 - MeasureText("READY", 20)/2, p2Y + 130, 20, BLACK);
            } else {
                DrawRectangle(p2X, p2Y + 115, 220, 50, RED);
                DrawText("NOT READY", p2X + 110 - MeasureText("NOT READY", 20)/2, p2Y + 130, 20, WHITE);
            }

            // Lobby Status Message
            if (p1Ready && p2Ready) {
                int startTextWidth = MeasureText("BOTH READY! STARTING GAME...", 24);
                DrawText("BOTH READY! STARTING GAME...", screen_width / 2 - startTextWidth / 2, panelY + 310, 24, YELLOW);
            } else {
                int waitTextWidth = MeasureText("Waiting for both players to be ready...", 20);
                DrawText("Waiting for both players to be ready...", screen_width / 2 - waitTextWidth / 2, panelY + 315, 20, LIGHTGRAY);
            }

            // Back Instruction
            int backTextWidth = MeasureText("Press 'B' to return to Main Menu | P: Pause | SPACE: Force Game Over", 20);
            DrawText("Press 'B' to return to Main Menu | P: Pause | SPACE: Force Game Over", screen_width / 2 - backTextWidth / 2, panelY + 385, 20, YELLOW);

            break;
        }

        case GameState::Multiplayer:
        {
            // Toggle pause state with 'P' key
            if (IsKeyPressed(KEY_P)) {
                isPaused = !isPaused;
            }

            // Force GameOver with SPACEBAR key
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::GameOver;
            }

            if (!isPaused) {
                sessionPlayTime += GetFrameTime();

                if (ball.Update() && sfxEnabled) {
                    PlaySound(wallHitSound);
                }
                player.Update();

                // Player 2 controls (Left Paddle -  Player 2 paddle)
                float dt = GetFrameTime();
                if (IsKeyDown(KEY_W)) {
                    cpu.position.y -= PLAYER_SPEED * dt;
                }
                if (IsKeyDown(KEY_S)) {
                    cpu.position.y += PLAYER_SPEED * dt;
                }
                cpu.LimitMovement();
                cpu.position.x = 20.0f + 10.0f; // Keep on left track

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
                    ball.velocity.x *= -1;
                    if (sfxEnabled) PlaySound(paddleHitSound);
                }

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
                    ball.velocity.x *= -1;
                    if (sfxEnabled) PlaySound(paddleHitSound);
                }

                if (ball.position.x + ball.radius >= screen_width - 20.0f) {
                    score.player2_score++;
                    if (sfxEnabled) PlaySound(scoreSound);
                    if (score.player2_score >= maxScore) {
                        currentState = GameState::GameOver;
                    }
                    else {
                        ResetBall(ball);
                    }
                }
                if (ball.position.x - ball.radius <= 20.0f) {
                    score.player_score++;
                    if (sfxEnabled) PlaySound(scoreSound);
                    if (score.player_score >= maxScore) {
                        currentState = GameState::GameOver;
                    }
                    else {
                        ResetBall(ball);
                    }
                }
            }

            // --- Draw Textured Court Background ---
            // Left Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ 20.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Right Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ (float)screen_width / 2.0f + 50.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Center Circle
            DrawCircle(screen_width / 2, screen_height / 2, 50.0f, Color{ 102, 51, 153, 100 });
            DrawCircleLines(screen_width / 2, screen_height / 2, 50.0f, Color{ 50, 25, 75, 250 });

            // Tiled Center Dashed Line (line.png)
            int lineY = 20;
            while (lineY < screen_height - 20) {
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

            // Scores: Left shows Player 2 score, Right shows Player 1 score
            DrawText(TextFormat("%i", score.player2_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", score.player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            // Draw Playtime Counter
            int minutes = (int)sessionPlayTime / 60;
            int seconds = (int)sessionPlayTime % 60;
            int timeTextWidth = MeasureText(TextFormat("%02i:%02i", minutes, seconds), 32);
            
            // Draw background box to block out the center line and increase contrast
            DrawRectangle(screen_width / 2 - timeTextWidth / 2 - 15, screen_height - 85, timeTextWidth + 30, 44, Color{ 15, 15, 15, 220 });
            DrawRectangleLines(screen_width / 2 - timeTextWidth / 2 - 15, screen_height - 85, timeTextWidth + 30, 44, Color{ 100, 100, 100, 255 });
            DrawText(TextFormat("%02i:%02i", minutes, seconds), screen_width / 2 - timeTextWidth / 2, screen_height - 79, 32, YELLOW);

            ball.Draw();
            cpu.Draw(); // Left paddle (reused cpu object)
            player.Draw(); // Right paddle

            // Draw Pause Overlay and Text
            if (isPaused) {
                DrawRectangle(20, 20, screen_width - 40, screen_height - 40, Color{ 0, 0, 0, 150 });
                
                int pausedTextWidth = MeasureText("PAUSED", 60);
                DrawText("PAUSED", screen_width / 2 - pausedTextWidth / 2, screen_height / 2 - 60, 60, YELLOW);
                
                int hintTextWidth = MeasureText("P1 (Right): UP/DOWN | P2 (Left): W/S | SPACE: Force Game Over", 20);
                DrawText("P1 (Right): UP/DOWN | P2 (Left): W/S | SPACE: Force Game Over", 
                         screen_width / 2 - hintTextWidth / 2, screen_height / 2 + 20, 20, WHITE);
            }

            break;
        }

        case GameState::Settings:
        {
            // Navigation
            if (IsKeyPressed(KEY_UP)) {
                selectedSettingLine--;
                if (selectedSettingLine < 0) selectedSettingLine = 5;
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selectedSettingLine++;
                if (selectedSettingLine > 5) selectedSettingLine = 0;
            }

            // Option selection (LEFT/RIGHT or ENTER)
            if (selectedSettingLine == 0) {
                // Resolution
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                    resolutionOption = (resolutionOption + 1) % 3;
                    if (resolutionOption == 0) ApplyResolution(1280, 800);
                    else if (resolutionOption == 1) ApplyResolution(1600, 900);
                    else if (resolutionOption == 2) ApplyResolution(1920, 1080);
                }
                else if (IsKeyPressed(KEY_LEFT)) {
                    resolutionOption = (resolutionOption - 1 + 3) % 3;
                    if (resolutionOption == 0) ApplyResolution(1280, 800);
                    else if (resolutionOption == 1) ApplyResolution(1600, 900);
                    else if (resolutionOption == 2) ApplyResolution(1920, 1080);
                }
            }
            else if (selectedSettingLine == 1) {
                // Framerate
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                    framerateOption = (framerateOption + 1) % 3;
                    ApplyFramerate(framerateOption);
                }
                else if (IsKeyPressed(KEY_LEFT)) {
                    framerateOption = (framerateOption - 1 + 3) % 3;
                    ApplyFramerate(framerateOption);
                }
            }
            else if (selectedSettingLine == 2) {
                // Screen Mode
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_ENTER)) {
                    ToggleFullscreen();
                    isFullscreen = !isFullscreen;
                }
            }
            else if (selectedSettingLine == 3) {
                // Score Limit
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                    maxScoreOption = (maxScoreOption + 1) % 4;
                }
                else if (IsKeyPressed(KEY_LEFT)) {
                    maxScoreOption = (maxScoreOption - 1 + 4) % 4;
                }
                
                if (maxScoreOption == 0) maxScore = 5;
                else if (maxScoreOption == 1) maxScore = 11;
                else if (maxScoreOption == 2) maxScore = 15;
                else if (maxScoreOption == 3) maxScore = 21;
            }
            else if (selectedSettingLine == 4) {
                // Sound Effects
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_ENTER)) {
                    sfxEnabled = !sfxEnabled;
                }
            }
            else if (selectedSettingLine == 5) {
                // Back
                if (IsKeyPressed(KEY_ENTER)) {
                    currentState = GameState::MainMenu;
                }
            }

            // Title
            int titleWidth = MeasureText("SETTINGS", 60);
            DrawText("SETTINGS", screen_width / 2 - titleWidth / 2, screen_height / 4 - 40, 60, WHITE);

            // Construct resolution string
            std::string resStr = "Resolution: ";
            if (resolutionOption == 0) resStr += "1280x800";
            else if (resolutionOption == 1) resStr += "1600x900";
            else if (resolutionOption == 2) resStr += "1920x1080 (Windowed)";

            // Construct framerate string
            std::string fpsStr = "Framerate: ";
            if (framerateOption == 0) fpsStr += "60 FPS";
            else if (framerateOption == 1) fpsStr += "144 FPS";
            else if (framerateOption == 2) fpsStr += "VSync";

            // Construct screen mode string
            std::string modeStr = "Screen Mode: ";
            if (isFullscreen) modeStr += "Fullscreen";
            else modeStr += "Windowed";

            // Construct score limit string
            std::string scoreLimitStr = "Score Limit: " + std::to_string(maxScore);

            // Construct sound effects string
            std::string sfxStr = "Sound Effects: ";
            if (sfxEnabled) sfxStr += "ON";
            else sfxStr += "OFF";

            // Draw option lines
            Color resColor = (selectedSettingLine == 0) ? YELLOW : WHITE;
            Color fpsColor = (selectedSettingLine == 1) ? YELLOW : WHITE;
            Color modeColor = (selectedSettingLine == 2) ? YELLOW : WHITE;
            Color scoreColor = (selectedSettingLine == 3) ? YELLOW : WHITE;
            Color sfxColor = (selectedSettingLine == 4) ? YELLOW : WHITE;
            Color backColor = (selectedSettingLine == 5) ? YELLOW : WHITE;

            int startY = screen_height / 2 - 90;
            DrawText(resStr.c_str(), screen_width / 2 - MeasureText(resStr.c_str(), 30) / 2, startY, 30, resColor);
            DrawText(fpsStr.c_str(), screen_width / 2 - MeasureText(fpsStr.c_str(), 30) / 2, startY + 45, 30, fpsColor);
            DrawText(modeStr.c_str(), screen_width / 2 - MeasureText(modeStr.c_str(), 30) / 2, startY + 90, 30, modeColor);
            DrawText(scoreLimitStr.c_str(), screen_width / 2 - MeasureText(scoreLimitStr.c_str(), 30) / 2, startY + 135, 30, scoreColor);
            DrawText(sfxStr.c_str(), screen_width / 2 - MeasureText(sfxStr.c_str(), 30) / 2, startY + 180, 30, sfxColor);
            DrawText("Back", screen_width / 2 - MeasureText("Back", 30) / 2, startY + 225, 30, backColor);

            // Controls helper
            int settingsHintWidth = MeasureText("UP/DOWN to navigate | LEFT/RIGHT to change settings | ENTER to select", 20);
            DrawText("UP/DOWN to navigate | LEFT/RIGHT to change settings | ENTER to select",
                screen_width / 2 - settingsHintWidth / 2,
                screen_height - 50, 20, WHITE);

            break;
        }

        case GameState::Playing:
        {
            // Toggle pause state with 'P' key
            if (IsKeyPressed(KEY_P)) {
                isPaused = !isPaused;
            }

            // Force GameOver with SPACEBAR key
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::GameOver;
            }

            if (!isPaused) {
                sessionPlayTime += GetFrameTime();

                if (ball.Update() && sfxEnabled) {
                    PlaySound(wallHitSound);
                }
                player.Update();
                cpu.Update(ball.position.y);

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
                    ball.velocity.x *= -1;
                    if (sfxEnabled) PlaySound(paddleHitSound);
                }

                if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
                    ball.velocity.x *= -1;
                    if (sfxEnabled) PlaySound(paddleHitSound);
                }

                if (ball.position.x + ball.radius >= screen_width - 20.0f) {
                    score.cpu_score++;
                    if (sfxEnabled) PlaySound(scoreSound);
                    if (score.cpu_score >= maxScore) {
                        currentState = GameState::GameOver;
                    }
                    else {
                        ResetBall(ball);
                    }
                }
                if (ball.position.x - ball.radius <= 20.0f) {
                    score.player_score++;
                    if (sfxEnabled) PlaySound(scoreSound);
                    if (score.player_score >= maxScore) {
                        currentState = GameState::GameOver;
                    }
                    else {
                        ResetBall(ball);
                    }
                }
            }

            // --- Draw Textured Court Background ---
            // Left Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ 20.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
                Vector2{ 0.0f, 0.0f },
                0.0f,
                WHITE
            );

            // Right Court (basic_space.png)
            DrawTexturePro(
                courtBackground,
                Rectangle{ 0.0f, 0.0f, (float)courtBackground.width, (float)courtBackground.height },
                Rectangle{ (float)screen_width / 2.0f + 50.0f, 20.0f, (float)screen_width / 2.0f - 70.0f, (float)screen_height - 40.0f },
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
            while (lineY < screen_height - 20) {
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

            DrawText(TextFormat("%i", score.cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", score.player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            // Draw Playtime Counter
            int minutes = (int)sessionPlayTime / 60;
            int seconds = (int)sessionPlayTime % 60;
            int timeTextWidth = MeasureText(TextFormat("%02i:%02i", minutes, seconds), 32);
            
            // Draw background box to block out the center line and increase contrast
            DrawRectangle(screen_width / 2 - timeTextWidth / 2 - 15, screen_height - 85, timeTextWidth + 30, 44, Color{ 15, 15, 15, 220 });
            DrawRectangleLines(screen_width / 2 - timeTextWidth / 2 - 15, screen_height - 85, timeTextWidth + 30, 44, Color{ 100, 100, 100, 255 });
            DrawText(TextFormat("%02i:%02i", minutes, seconds), screen_width / 2 - timeTextWidth / 2, screen_height - 79, 32, YELLOW);

            ball.Draw();
            cpu.Draw();
            player.Draw();

            // Draw Pause Overlay and Text
            if (isPaused) {
                // Semi-transparent overlay inside the borders
                DrawRectangle(20, 20, screen_width - 40, screen_height - 40, Color{ 0, 0, 0, 150 });
                
                int pausedTextWidth = MeasureText("PAUSED", 60);
                DrawText("PAUSED", screen_width / 2 - pausedTextWidth / 2, screen_height / 2 - 60, 60, YELLOW);
                
                int hintTextWidth = MeasureText("UP/DOWN Arrows to move | SPACE: Force Game Over", 20);
                DrawText("UP/DOWN Arrows to move | SPACE: Force Game Over", 
                         screen_width / 2 - hintTextWidth / 2, screen_height / 2 + 20, 20, WHITE);
            }

            break;
        }
        case GameState::GameOver:
        {
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = GameState::MainMenu;
            }

            // Draw a nice centered dark panel for game over info
            int panelWidth = 600;
            int panelHeight = 400;
            int panelX = screen_width / 2 - panelWidth / 2;
            int panelY = screen_height / 2 - panelHeight / 2;

            // Semi-transparent background panel
            DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{ 15, 15, 15, 230 });
            DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{ 120, 120, 120, 255 });

            // Draw "GAME OVER" header
            int gameOverWidth = MeasureText("GAME OVER", 60);
            DrawText("GAME OVER", screen_width / 2 - gameOverWidth / 2, panelY + 40, 60, RED);

            // Determine winner text and color
            std::string winnerText = "GAME ENDED";
            Color winnerColor = YELLOW;
            std::string score1Str, score2Str;

            if (isMultiplayer) {
                if (score.player_score > score.player2_score) {
                    winnerText = "PLAYER 1 WINS!";
                    winnerColor = Green;
                }
                else if (score.player2_score > score.player_score) {
                    winnerText = "PLAYER 2 WINS!";
                    winnerColor = RED;
                }
                score1Str = "Player 1 Score: " + std::to_string(score.player_score);
                score2Str = "Player 2 Score: " + std::to_string(score.player2_score);
            }
            else {
                if (score.player_score > score.cpu_score) {
                    winnerText = "YOU WIN!";
                    winnerColor = Green;
                }
                else if (score.cpu_score > score.player_score) {
                    winnerText = "CPU WINS!";
                    winnerColor = RED;
                }
                score1Str = "Player Score: " + std::to_string(score.player_score);
                score2Str = "CPU Score: " + std::to_string(score.cpu_score);
            }

            int winnerWidth = MeasureText(winnerText.c_str(), 40);
            DrawText(winnerText.c_str(), screen_width / 2 - winnerWidth / 2, panelY + 120, 40, winnerColor);

            // Draw final scores
            int playerTextWidth = MeasureText(score1Str.c_str(), 30);
            int cpuTextWidth = MeasureText(score2Str.c_str(), 30);

            DrawText(score1Str.c_str(), screen_width / 2 - playerTextWidth / 2, panelY + 200, 30, WHITE);
            DrawText(score2Str.c_str(), screen_width / 2 - cpuTextWidth / 2, panelY + 250, 30, WHITE);

            // Draw playtime
            int minutes = (int)sessionPlayTime / 60;
            int seconds = (int)sessionPlayTime % 60;
            std::string timeStr = "Playtime: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
            int timeTextWidth = MeasureText(timeStr.c_str(), 20);
            DrawText(timeStr.c_str(), screen_width / 2 - timeTextWidth / 2, panelY + 300, 20, LIGHTGRAY);

            // Instruction to return
            int instWidth = MeasureText("Press SPACEBAR to return to Main Menu", 20);
            DrawText("Press SPACEBAR to return to Main Menu", screen_width / 2 - instWidth / 2, panelY + 340, 20, YELLOW);

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

    UnloadSound(paddleHitSound);
    UnloadSound(wallHitSound);
    UnloadSound(scoreSound);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}