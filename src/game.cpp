#include "game.h"

// --- Paddle Implementation ---

void Paddle::Update() {
    float speed = 6.0f;

    if (IsKeyDown(KEY_UP)) {
        position.y -= speed;
    }
    if (IsKeyDown(KEY_DOWN)) {
        position.y += speed;
    }

    // Limit movement
    if (position.y <= 0) {
        position.y = 0;
    }
    if (position.y + height >= GetScreenHeight()) {
        position.y = GetScreenHeight() - height;
    }
}

void Paddle::Draw() {
    DrawRectangleRounded(Rectangle{ position.x, position.y, width, height }, 0.8f, 0, color);
}


// --- Ball Implementation ---

void Ball::Update() {
    position.x += velocity.x;
    position.y += velocity.y;

    if (position.y + radius >= GetScreenHeight() || position.y - radius <= 0) {
        velocity.y *= -1;
    }
}

void Ball::Draw() {
    // Cast to int as DrawCircle expects integers for coordinates
    DrawCircle((int)position.x, (int)position.y, radius, color);
}