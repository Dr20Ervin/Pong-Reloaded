#pragma once

#include <raylib.h>
#include <string>
#include <vector>

// Game States & Configurations
enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

constexpr float CPU_SPEED_EASY = 3.0f;
constexpr float CPU_SPEED_NORMAL = 5.5f;
constexpr float CPU_SPEED_HARD = 8.0f;

// --- Base Entities ---

class GameObject {
public:
    Vector2 position;
    Color color;

public:
    GameObject(Vector2 pos, Color c) : position(pos), color(c) {}
    virtual ~GameObject() = default;

    virtual void Update() = 0;
    virtual void Draw() = 0;
};

// --- Game Objects ---

class Paddle : public GameObject {
public:
    float width;
    float height;

    Paddle(Vector2 pos, Color c, float w, float h)
        : GameObject(pos, c), width(w), height(h) {
    }

    void Update() override;
    void Draw() override;
};

class Ball : public GameObject {
public:
    float radius;
    Vector2 velocity;

    Ball(Vector2 pos, Color c, float r)
        : GameObject(pos, c), radius(r), velocity({ 5.0f, 5.0f }) {
    }

    void Update() override;
    void Draw() override;
};

class CpuPaddle : public Paddle {
private:
    float speed;
    Difficulty currentDifficulty;

public:
    CpuPaddle(Vector2 pos, Color c, float w, float h, Difficulty diff = Difficulty::Normal)
        : Paddle(pos, c, w, h) {
        SetDifficulty(diff);
    }

    void SetDifficulty(Difficulty diff) {
        currentDifficulty = diff;
        switch (currentDifficulty) {
        case Difficulty::Easy:   speed = CPU_SPEED_EASY; break;
        case Difficulty::Normal: speed = CPU_SPEED_NORMAL; break;
        case Difficulty::Hard:   speed = CPU_SPEED_HARD; break;
        }
    }

    void Update(float ball_y) {
        float paddleCenter = position.y + (height / 2.0f);

        if (paddleCenter > ball_y + speed) {
            position.y -= speed;
        }
        else if (paddleCenter < ball_y - speed) {
            position.y += speed;
        }

        LimitMovement();
    }

    void Update() override {}

    void LimitMovement() {
        if (position.y <= 0) {
            position.y = 0;
        }
        if (position.y + height >= GetScreenHeight()) {
            position.y = GetScreenHeight() - height;
        }
    }
};