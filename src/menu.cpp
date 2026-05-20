#include "menu.h"

void Menu::Update(GameState& currentState) {
    // Handle Navigation
    if (IsKeyPressed(KEY_DOWN)) {
        selectedIndex++;
        if (selectedIndex >= options.size()) selectedIndex = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = options.size() - 1;
    }

    // Handle Selection
    if (IsKeyPressed(KEY_ENTER)) {
        if (options[selectedIndex] == "Start Game") {
            currentState = GameState::Playing;
        }
        else if (options[selectedIndex] == "Quit") {
            currentState = GameState::GameOver;
        }
    }
}

void Menu::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Draw Title
    DrawText(title.c_str(), screenWidth / 2 - MeasureText(title.c_str(), 60) / 2, screenHeight / 4, 60, WHITE);

    // Draw Options
    for (int i = 0; i < options.size(); i++) {
        Color textColor = (i == selectedIndex) ? YELLOW : WHITE;
        DrawText(options[i].c_str(), screenWidth / 2 - MeasureText(options[i].c_str(), 40) / 2, screenHeight / 2 + (i * 60), 40, textColor);
    }
}