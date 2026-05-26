#include "menu.h"
#include "main.h"
#include "resource.h"

int Menu::Update() {
    // Menu navigation
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= static_cast<int>(options.size())) selectedIndex = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = static_cast<int>(options.size()) - 1;
    }

    // Option selection
    if (IsKeyPressed(KEY_ENTER)) {
        return selectedIndex;
    }
    return -1;
}

void Menu::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Render Menu Title (aligned down for balanced spacing)
    DrawText(title.c_str(), screenWidth / 2 - MeasureText(title.c_str(), 60) / 2, screenHeight / 4 + 20, 60, WHITE);

    int numOptions = static_cast<int>(options.size());
    int spacing = 60;
    
    // Isolate "Back" buttons at the bottom; group all other menu items in the center
    bool separateLast = (!options.empty() && options.back() == "Back");
    int groupCount = separateLast ? (numOptions - 1) : numOptions;
    
    // Center grouped options vertically
    int startY = (screenHeight / 2) - (((groupCount - 1) * spacing) / 2);

    for (int i = 0; i < numOptions; i++) {
        Color textColor = (i == selectedIndex) ? YELLOW : WHITE;
        if (separateLast && i == numOptions - 1) {
            // Render separated "Back" action button at the bottom of the screen
            DrawText(options[i].c_str(), screenWidth / 2 - MeasureText(options[i].c_str(), 40) / 2, screenHeight - 120, 40, textColor);
        } else {
            // Render standard grouped options in the center
            DrawText(options[i].c_str(), screenWidth / 2 - MeasureText(options[i].c_str(), 40) / 2, startY + (i * spacing), 40, textColor);
        }
    }
}

// State routines for settings and multiplayer lobby

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

void UpdateSettingsState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu) {
    // Menu navigation
    if (IsKeyPressed(KEY_UP)) {
        ctx.config.selectedSettingLine--;
        if (ctx.config.selectedSettingLine < 0) ctx.config.selectedSettingLine = 8;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        ctx.config.selectedSettingLine++;
        if (ctx.config.selectedSettingLine > 8) ctx.config.selectedSettingLine = 0;
    }

    // Settings adjustments (LEFT/RIGHT or ENTER)
    if (ctx.config.selectedSettingLine == 0) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.resolutionOption = (ctx.config.resolutionOption + 1) % 3;
            if (ctx.config.resolutionOption == 0) ApplyResolution(1280, 800);
            else if (ctx.config.resolutionOption == 1) ApplyResolution(1600, 900);
            else if (ctx.config.resolutionOption == 2) ApplyResolution(1920, 1080);
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.resolutionOption = (ctx.config.resolutionOption - 1 + 3) % 3;
            if (ctx.config.resolutionOption == 0) ApplyResolution(1280, 800);
            else if (ctx.config.resolutionOption == 1) ApplyResolution(1600, 900);
            else if (ctx.config.resolutionOption == 2) ApplyResolution(1920, 1080);
        }
    }
    else if (ctx.config.selectedSettingLine == 1) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.framerateOption = (ctx.config.framerateOption + 1) % 3;
            ApplyFramerate(ctx.config.framerateOption);
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.framerateOption = (ctx.config.framerateOption - 1 + 3) % 3;
            ApplyFramerate(ctx.config.framerateOption);
        }
    }
    else if (ctx.config.selectedSettingLine == 2) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_ENTER)) {
            ToggleFullscreen();
            ctx.config.isFullscreen = !ctx.config.isFullscreen;
        }
    }
    else if (ctx.config.selectedSettingLine == 3) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.maxScoreOption = (ctx.config.maxScoreOption + 1) % 4;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.maxScoreOption = (ctx.config.maxScoreOption - 1 + 4) % 4;
        }
        
        if (ctx.config.maxScoreOption == 0) ctx.config.maxScore = 5;
        else if (ctx.config.maxScoreOption == 1) ctx.config.maxScore = 11;
        else if (ctx.config.maxScoreOption == 2) ctx.config.maxScore = 15;
        else if (ctx.config.maxScoreOption == 3) ctx.config.maxScore = 21;
    }
    else if (ctx.config.selectedSettingLine == 4) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.sfxEnabled = !ctx.config.sfxEnabled;
        }
    }
    else if (ctx.config.selectedSettingLine == 5) {
        bool changed = false;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.spaceThemeOption = (ctx.config.spaceThemeOption + 1) % 3;
            changed = true;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.spaceThemeOption = (ctx.config.spaceThemeOption - 1 + 3) % 3;
            changed = true;
        }
        if (changed) {
            int resId = IDR_TEX_SPACE_DEFAULT;
            if (ctx.config.spaceThemeOption == 1) resId = IDR_TEX_SPACE_GALAXY;
            else if (ctx.config.spaceThemeOption == 2) resId = IDR_TEX_SPACE_TRAP;

            if (ctx.courtBackground.id > 0) {
                UnloadTexture(ctx.courtBackground);
            }
            ctx.courtBackground = LoadTextureFromResource(resId);
        }
    }
    else if (ctx.config.selectedSettingLine == 6) {
        bool changed = false;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.ballThemeOption = (ctx.config.ballThemeOption + 1) % 5;
            changed = true;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.ballThemeOption = (ctx.config.ballThemeOption - 1 + 5) % 5;
            changed = true;
        }
        if (changed) {
            int resId = IDR_TEX_BALL_DEFAULT;
            if (ctx.config.ballThemeOption == 1) resId = IDR_TEX_BALL_BANANA;
            else if (ctx.config.ballThemeOption == 2) resId = IDR_TEX_BALL_BEACH;
            else if (ctx.config.ballThemeOption == 3) resId = IDR_TEX_BALL_CLOUDY;
            else if (ctx.config.ballThemeOption == 4) resId = IDR_TEX_BALL_SIMPLE;

            ball.ReloadTexture(resId);
        }
    }
    else if (ctx.config.selectedSettingLine == 7) {
        bool changed = false;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
            ctx.config.paddleThemeOption = (ctx.config.paddleThemeOption + 1) % 4;
            changed = true;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            ctx.config.paddleThemeOption = (ctx.config.paddleThemeOption - 1 + 4) % 4;
            changed = true;
        }
        if (changed) {
            int resId = IDR_TEX_PADDLE_DEFAULT;
            if (ctx.config.paddleThemeOption == 1) resId = IDR_TEX_PADDLE_CLOUDY;
            else if (ctx.config.paddleThemeOption == 2) resId = IDR_TEX_PADDLE_CPU;
            else if (ctx.config.paddleThemeOption == 3) resId = IDR_TEX_PADDLE_HUMAN;

            player.ReloadTexture(resId);
            cpu.ReloadTexture(resId);
        }
    }
    else if (ctx.config.selectedSettingLine == 8) {
        if (IsKeyPressed(KEY_ENTER)) {
            ctx.currentState = GameState::MainMenu;
        }
    }
}

void DrawSettingsState(const GameContext& ctx, int screenWidth, int screenHeight) {
    int titleWidth = MeasureText("SETTINGS", 60);
    DrawText("SETTINGS", screenWidth / 2 - titleWidth / 2, screenHeight / 4 + 20, 60, WHITE);

    // Option labels formatting
    std::string resStr = "Resolution: ";
    if (ctx.config.resolutionOption == 0) resStr += "1280x800";
    else if (ctx.config.resolutionOption == 1) resStr += "1600x900";
    else if (ctx.config.resolutionOption == 2) resStr += "1920x1080 (Windowed)";

    std::string fpsStr = "Framerate: ";
    if (ctx.config.framerateOption == 0) fpsStr += "60 FPS";
    else if (ctx.config.framerateOption == 1) fpsStr += "144 FPS";
    else if (ctx.config.framerateOption == 2) fpsStr += "VSync";

    std::string modeStr = "Screen Mode: ";
    if (ctx.config.isFullscreen) modeStr += "Fullscreen";
    else modeStr += "Windowed";

    std::string scoreLimitStr = "Score Limit: " + std::to_string(ctx.config.maxScore);

    std::string sfxStr = "Sound Effects: ";
    if (ctx.config.sfxEnabled) sfxStr += "ON";
    else sfxStr += "OFF";

    std::string spaceThemeStr = "Space Theme: ";
    if (ctx.config.spaceThemeOption == 0) spaceThemeStr += "Default";
    else if (ctx.config.spaceThemeOption == 1) spaceThemeStr += "Galaxy";
    else if (ctx.config.spaceThemeOption == 2) spaceThemeStr += "Trap";

    std::string ballThemeStr = "Ball Theme: ";
    if (ctx.config.ballThemeOption == 0) ballThemeStr += "Default";
    else if (ctx.config.ballThemeOption == 1) ballThemeStr += "Banana";
    else if (ctx.config.ballThemeOption == 2) ballThemeStr += "Beach";
    else if (ctx.config.ballThemeOption == 3) ballThemeStr += "Cloudy";
    else if (ctx.config.ballThemeOption == 4) ballThemeStr += "Simple";

    std::string paddleThemeStr = "Paddle Theme: ";
    if (ctx.config.paddleThemeOption == 0) paddleThemeStr += "Default";
    else if (ctx.config.paddleThemeOption == 1) paddleThemeStr += "Cloudy";
    else if (ctx.config.paddleThemeOption == 2) paddleThemeStr += "CPU Style";
    else if (ctx.config.paddleThemeOption == 3) paddleThemeStr += "Human Style";

    // Menu options highlighting and drawing
    Color resColor = (ctx.config.selectedSettingLine == 0) ? YELLOW : WHITE;
    Color fpsColor = (ctx.config.selectedSettingLine == 1) ? YELLOW : WHITE;
    Color modeColor = (ctx.config.selectedSettingLine == 2) ? YELLOW : WHITE;
    Color scoreColor = (ctx.config.selectedSettingLine == 3) ? YELLOW : WHITE;
    Color sfxColor = (ctx.config.selectedSettingLine == 4) ? YELLOW : WHITE;
    Color spaceColor = (ctx.config.selectedSettingLine == 5) ? YELLOW : WHITE;
    Color ballColor = (ctx.config.selectedSettingLine == 6) ? YELLOW : WHITE;
    Color paddleColor = (ctx.config.selectedSettingLine == 7) ? YELLOW : WHITE;
    Color backColor = (ctx.config.selectedSettingLine == 8) ? YELLOW : WHITE;

    // Render settings toggles centered in the middle of the screen
    int startY = 300;
    DrawText(resStr.c_str(), screenWidth / 2 - MeasureText(resStr.c_str(), 30) / 2, startY, 30, resColor);
    DrawText(fpsStr.c_str(), screenWidth / 2 - MeasureText(fpsStr.c_str(), 30) / 2, startY + 40, 30, fpsColor);
    DrawText(modeStr.c_str(), screenWidth / 2 - MeasureText(modeStr.c_str(), 30) / 2, startY + 80, 30, modeColor);
    DrawText(scoreLimitStr.c_str(), screenWidth / 2 - MeasureText(scoreLimitStr.c_str(), 30) / 2, startY + 120, 30, scoreColor);
    DrawText(sfxStr.c_str(), screenWidth / 2 - MeasureText(sfxStr.c_str(), 30) / 2, startY + 160, 30, sfxColor);
    DrawText(spaceThemeStr.c_str(), screenWidth / 2 - MeasureText(spaceThemeStr.c_str(), 30) / 2, startY + 200, 30, spaceColor);
    DrawText(ballThemeStr.c_str(), screenWidth / 2 - MeasureText(ballThemeStr.c_str(), 30) / 2, startY + 240, 30, ballColor);
    DrawText(paddleThemeStr.c_str(), screenWidth / 2 - MeasureText(paddleThemeStr.c_str(), 30) / 2, startY + 280, 30, paddleColor);

    // Render separated "Back" action button at the bottom of the screen
    DrawText("Back", screenWidth / 2 - MeasureText("Back", 30) / 2, screenHeight - 120, 30, backColor);

    // Render bottom hint for settings controls
    int settingsHintWidth = MeasureText("UP/DOWN to navigate | LEFT/RIGHT to change settings | ENTER to select", 20);
    DrawText("UP/DOWN to navigate | LEFT/RIGHT to change settings | ENTER to select",
        screenWidth / 2 - settingsHintWidth / 2,
        screenHeight - 50, 20, WHITE);
}

void UpdateLobbyState(GameContext& ctx) {
    if (IsKeyPressed(KEY_W)) {
        ctx.p1Ready = !ctx.p1Ready;
        if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
    }
    if (IsKeyPressed(KEY_UP)) {
        ctx.p2Ready = !ctx.p2Ready;
        if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
    }
    if (IsKeyPressed(KEY_B)) {
        ctx.currentState = GameState::MainMenu;
    }

    if (ctx.p1Ready && ctx.p2Ready) {
        ctx.currentState = GameState::Multiplayer;
    }
}

void DrawLobbyState(const GameContext& ctx, int screenWidth, int screenHeight) {
    // Draw background field
    DrawCourt(ctx, screenWidth, screenHeight);

    // Render lobby overlay card
    int panelWidth = 800;
    int panelHeight = 450;
    int panelX = screenWidth / 2 - panelWidth / 2;
    int panelY = screenHeight / 2 - panelHeight / 2;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{ 15, 15, 15, 230 });
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{ 120, 120, 120, 255 });

    int titleWidth = MeasureText("MULTIPLAYER LOBBY", 40);
    DrawText("MULTIPLAYER LOBBY", screenWidth / 2 - titleWidth / 2, panelY + 30, 40, WHITE);

    // Player 1 section
    int p1X = panelX + 50;
    int p1Y = panelY + 110;
    DrawText("PLAYER 1 (LEFT)", p1X, p1Y, 24, LIGHTGRAY);
    DrawText("Controls: W / S", p1X, p1Y + 45, 20, WHITE);
    DrawText("Press 'W' to toggle ready", p1X, p1Y + 75, 18, GRAY);
    
    if (ctx.p1Ready) {
        DrawRectangle(p1X, p1Y + 115, 220, 50, Green);
        DrawText("READY", p1X + 110 - MeasureText("READY", 20)/2, p1Y + 130, 20, BLACK);
    } else {
        DrawRectangle(p1X, p1Y + 115, 220, 50, RED);
        DrawText("NOT READY", p1X + 110 - MeasureText("NOT READY", 20)/2, p1Y + 130, 20, WHITE);
    }

    // Player 2 section
    int p2X = panelX + panelWidth - 270;
    int p2Y = panelY + 110;
    DrawText("PLAYER 2 (RIGHT)", p2X, p2Y, 24, LIGHTGRAY);
    DrawText("Controls: UP / DOWN", p2X, p2Y + 45, 20, WHITE);
    DrawText("Press 'UP' to toggle ready", p2X, p2Y + 75, 18, GRAY);
    
    if (ctx.p2Ready) {
        DrawRectangle(p2X, p2Y + 115, 220, 50, Green);
        DrawText("READY", p2X + 110 - MeasureText("READY", 20)/2, p2Y + 130, 20, BLACK);
    } else {
        DrawRectangle(p2X, p2Y + 115, 220, 50, RED);
        DrawText("NOT READY", p2X + 110 - MeasureText("NOT READY", 20)/2, p2Y + 130, 20, WHITE);
    }

    // Ready status message
    if (ctx.p1Ready && ctx.p2Ready) {
        int startTextWidth = MeasureText("BOTH READY! STARTING GAME...", 24);
        DrawText("BOTH READY! STARTING GAME...", screenWidth / 2 - startTextWidth / 2, panelY + 310, 24, YELLOW);
    } else {
        int waitTextWidth = MeasureText("Waiting for both players to be ready...", 20);
        DrawText("Waiting for both players to be ready...", screenWidth / 2 - waitTextWidth / 2, panelY + 315, 20, LIGHTGRAY);
    }

    int backTextWidth = MeasureText("Press 'B' to return to Main Menu | P: Pause | SPACE: Force Game Over", 20);
    DrawText("Press 'B' to return to Main Menu | P: Pause | SPACE: Force Game Over", screenWidth / 2 - backTextWidth / 2, panelY + 385, 20, YELLOW);
}
