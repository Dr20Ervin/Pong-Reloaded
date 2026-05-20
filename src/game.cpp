#include "game.h"

// --- Paddle Implementation ---

bool Paddle::Update() {
    float dt = GetFrameTime();

    if (IsKeyDown(KEY_UP)) {
        position.y -= PLAYER_SPEED * dt;
    }
    if (IsKeyDown(KEY_DOWN)) {
        position.y += PLAYER_SPEED * dt;
    }

    // Limit movement
    if (position.y <= 20.0f) {
        position.y = 20.0f;
    }
    if (position.y + height >= GetScreenHeight() - 20.0f) {
        position.y = GetScreenHeight() - 20.0f - height;
    }

    // Dynamic X position
    position.x = GetScreenWidth() - 20.0f - 10.0f - width;

    return false;
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

bool Ball::Update() {
    float dt = GetFrameTime();
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (position.y + radius >= GetScreenHeight() - 20.0f || position.y - radius <= 20.0f) {
        velocity.y *= -1;
        return true;
    }
    return false;
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