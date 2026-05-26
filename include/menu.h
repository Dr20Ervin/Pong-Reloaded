#pragma once

#include "game.h" 
#include <string>
#include <vector>

// Menu UI controller
class Menu {
private:
    std::string title;
    std::vector<std::string> options;
    int selectedIndex;

public:
    Menu(std::string menuTitle, std::vector<std::string> menuOptions)
        : title(menuTitle), options(menuOptions), selectedIndex(0) {
    }

    int Update();
    void Draw();
};

// Settings and lobby routines
void ApplyResolution(int width, int height);
void ApplyFramerate(int option);
void UpdateSettingsState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu);
void DrawSettingsState(const GameContext& ctx, int screenWidth, int screenHeight);
void UpdateLobbyState(GameContext& ctx);
void DrawLobbyState(const GameContext& ctx, int screenWidth, int screenHeight);
