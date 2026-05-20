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
    if (position.y <= 20.0f) {
        position.y = 20.0f;
    }
    if (position.y + height >= GetScreenHeight() - 20.0f) {
        position.y = GetScreenHeight() - 20.0f - height;
    }
}

void Paddle::Draw() {
    if (texture.id > 0) {
        DrawTexturePro(
            texture,
            Rectangle{ 0.0f, 0.0f, (float)texture.width, (float)texture.height },
            Rectangle{ position.x, position.y, width, height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRounded(Rectangle{ position.x, position.y, width, height }, 0.8f, 0, color);
    }
}


// --- Ball Implementation ---

void Ball::Update() {
    position.x += velocity.x;
    position.y += velocity.y;

    if (position.y + radius >= GetScreenHeight() - 20.0f || position.y - radius <= 20.0f) {
        velocity.y *= -1;
    }
}

void Ball::Draw() {
    if (texture.id > 0) {
        DrawTexturePro(
            texture,
            Rectangle{ 0.0f, 0.0f, (float)texture.width, (float)texture.height },
            Rectangle{ position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        // Cast to int as DrawCircle expects integers for coordinates
        DrawCircle((int)position.x, (int)position.y, radius, color);
    }
}