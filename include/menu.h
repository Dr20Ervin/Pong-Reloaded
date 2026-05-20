#pragma once
#include "game.h" 
#include <string>
#include <vector>

// --- UI / Systems ---

class Menu {
private:
    std::string title;
    std::vector<std::string> options;
    int selectedIndex;

public:
    Menu(std::string menuTitle, std::vector<std::string> menuOptions)
        : title(menuTitle), options(menuOptions), selectedIndex(0) {
    }

    void Update(GameState& currentState);
    void Draw();
};