#include "menu.h"

int Menu::Update() {
    // Handle Navigation
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= static_cast<int>(options.size())) selectedIndex = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = static_cast<int>(options.size()) - 1;
    }

    // Handle Selection
    if (IsKeyPressed(KEY_ENTER)) {
        return selectedIndex;
    }
    return -1;
}

void Menu::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Draw Title
    DrawText(title.c_str(), screenWidth / 2 - MeasureText(title.c_str(), 60) / 2, screenHeight / 4, 60, WHITE);

    // Draw Options
    for (int i = 0; i < static_cast<int>(options.size()); i++) {
        Color textColor = (i == selectedIndex) ? YELLOW : WHITE;
        DrawText(options[i].c_str(), screenWidth / 2 - MeasureText(options[i].c_str(), 40) / 2, screenHeight / 2 + (i * 60), 40, textColor);
    }

    // Draw Control keys
}