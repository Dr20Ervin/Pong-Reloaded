#include "main.h"
#include "game.h"
#include "menu.h"
#include "resource.h"

// Application Version Metadata
constexpr const char* gameVersion = "1.0";

// Global Theme Color Configurations
Color Green       = Color{ 38, 185, 154, 255 };
Color Dark_Green  = Color{ 20, 160, 133, 255 };
Color Light_Green = Color{ 129, 204, 184, 255 };
Color Yellow      = Color{ 243, 213, 91, 255 };

std::vector<std::string> logHistory;
std::mutex logMutex;
bool isConsoleVisible = false;

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
extern "C" {
    int __stdcall AllocConsole();
    int __stdcall FreeConsole();
}
#endif

int main() {
    std::cout << "Starting game session" << std::endl;
    int screen_width = 1280;
    int screen_height = 800;

    SetTraceLogCallback(CustomLogCallback);
    InitWindow(screen_width, screen_height, "Pong Reloaded");
    SetWindowIconFromResource(IDI_ICON1);
    SetTargetFPS(60);
    InitAudioDevice();

    // App state context
    GameContext ctx;

    // Asset textures loading
    ctx.courtBackground = LoadTextureFromResource(IDR_TEX_SPACE_DEFAULT);
    ctx.wallsTexture = LoadTextureFromResource(IDR_TEX_WALLS);
    ctx.lineTexture = LoadTextureFromResource(IDR_TEX_LINE);

    // Audio assets loading
    ctx.paddleHitSound = LoadSoundFromResource(IDR_SND_PADDLE_HIT);
    ctx.wallHitSound = LoadSoundFromResource(IDR_SND_WALL_HIT);
    ctx.scoreSound = LoadSoundFromResource(IDR_SND_SCORE);

    // Entities instantiation
    Ball ball(
        Vector2{ screen_width / 2.0f, screen_height / 2.0f }, 
        Yellow, 20.0f, 
        IDR_TEX_BALL_DEFAULT
    );
    ResetBall(ball);

    Paddle player(
        Vector2{ screen_width - 20.0f - 10.0f - 25.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f,
        IDR_TEX_PADDLE_DEFAULT
    );

    CpuPaddle cpu(
        Vector2{ 20.0f + 10.0f, screen_height / 2.0f - 60.0f },
        WHITE, 25.0f, 120.0f,
        Difficulty::Normal,
        IDR_TEX_PADDLE_DEFAULT
    );

    // Menu instantiation
    Menu mainMenu("PONG RELOADED", { "Singleplayer", "Multiplayer", "Settings", "Quit" });
    Menu difficultyMenu("SELECT DIFFICULTY", { "Easy", "Normal", "Hard", "Back" });

    // Main loop
    while (WindowShouldClose() == false && !ctx.shouldQuit) {
        screen_width = GetScreenWidth();
        screen_height = GetScreenHeight();

#ifdef _WIN32
        if (IsKeyPressed(KEY_F7)) {
            isConsoleVisible = !isConsoleVisible;
            if (isConsoleVisible) {
                AllocConsole();
                FILE* dummy;
                freopen_s(&dummy, "CONOUT$", "w", stdout);
                freopen_s(&dummy, "CONOUT$", "w", stderr);
                
                // Dump history
                std::lock_guard<std::mutex> lock(logMutex);
                for (const auto& log : logHistory) {
                    printf("%s", log.c_str());
                }
            } else {
                FreeConsole();
            }
        }
#endif

        // Update loop
        switch (ctx.currentState) {
        case GameState::MainMenu:
        {
            int selected = mainMenu.Update();
            if (selected == 0) {
                TraceLog(LOG_INFO, "State Transition: MainMenu -> DifficultySelect (Singleplayer)");
                ctx.isMultiplayer = false;
                ctx.currentState = GameState::DifficultySelect;
            }
            else if (selected == 1) {
                TraceLog(LOG_INFO, "State Transition: MainMenu -> MultiplayerLobby");
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
                if (selected == 0) { cpu.SetDifficulty(Difficulty::Easy); TraceLog(LOG_INFO, "CPU Difficulty set to: EASY"); }
                else if (selected == 1) { cpu.SetDifficulty(Difficulty::Normal); TraceLog(LOG_INFO, "CPU Difficulty set to: NORMAL"); }
                else if (selected == 2) { cpu.SetDifficulty(Difficulty::Hard); TraceLog(LOG_INFO, "CPU Difficulty set to: HARD"); }

                TraceLog(LOG_INFO, "State Transition: DifficultySelect -> Playing");
                ctx.score.player_score = 0;
                ctx.score.player2_score = 0;
                ctx.score.cpu_score = 0;
                ctx.sessionPlayTime = 0.0f;
                ctx.isPaused = false;
                ResetBall(ball);
                ctx.currentState = GameState::Playing;
                ApplyPaddleTextures(ctx, player, cpu);
            }
            else if (selected == 3) {
                ctx.currentState = GameState::MainMenu;
            }
            break;
        }
        case GameState::MultiplayerLobby:
        {
            GameState oldState = ctx.currentState;
            UpdateLobbyState(ctx);
            if (ctx.currentState == GameState::Multiplayer && oldState != GameState::Multiplayer) {
                ApplyPaddleTextures(ctx, player, cpu);
            }
            break;
        }
        case GameState::Multiplayer:
            UpdateMultiplayerState(ctx, ball, player, cpu);
            break;
        case GameState::Settings:
            UpdateSettingsState(ctx, ball, player, cpu);
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
            
            // Render navigation controls hint
            int menuHintWidth = MeasureText("Use UP/DOWN to navigate | ENTER to select", 20);
            DrawText("Use UP/DOWN to navigate | ENTER to select",
                screen_width / 2 - menuHintWidth / 2,
                screen_height - 80, 20, WHITE);
            
            // Render version string in bottom-left corner
            DrawText(TextFormat("v%s", gameVersion), 20, screen_height - 40, 20, LIGHTGRAY);
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

void CustomLogCallback(int logLevel, const char* text, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), text, args);

    std::string logStr;
    switch (logLevel) {
    case LOG_TRACE:   logStr = "TRACE: "; break;
    case LOG_DEBUG:   logStr = "DEBUG: "; break;
    case LOG_INFO:    logStr = "INFO: "; break;
    case LOG_WARNING: logStr = "WARNING: "; break;
    case LOG_ERROR:   logStr = "ERROR: "; break;
    case LOG_FATAL:   logStr = "FATAL: "; break;
    default:          logStr = "LOG: "; break;
    }
    logStr += buffer;
    logStr += "\n";

    std::lock_guard<std::mutex> lock(logMutex);
    logHistory.push_back(logStr);

#ifdef _WIN32
    if (isConsoleVisible) {
        printf("%s", logStr.c_str());
    }
#else
    printf("%s", logStr.c_str());
#endif
}