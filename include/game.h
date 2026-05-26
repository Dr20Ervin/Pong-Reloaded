#pragma once

#include <raylib.h>
#include <string>
#include <vector>

// Game states and configurations
enum class GameState {
    MainMenu,
    DifficultySelect,
    MultiplayerLobby,
    Multiplayer,
    Settings,
    Playing,
    Paused,
    GameOver
};

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

constexpr float CPU_SPEED_EASY = 180.0f;
constexpr float CPU_SPEED_NORMAL = 330.0f;
constexpr float CPU_SPEED_HARD = 480.0f;
constexpr float PLAYER_SPEED = 360.0f;
constexpr float BALL_SPEED = 420.0f;

// Game state contexts
struct ScoreBoard {
    int player_score = 0;
    int player2_score = 0;
    int cpu_score = 0;
};

struct GameConfig {
    int resolutionOption = 0;      // 0 = 1280x800, 1 = 1600x900, 2 = 1920x1080
    int framerateOption = 0;       // 0 = 60 FPS, 1 = 144 FPS, 2 = VSync
    bool isFullscreen = false;
    int maxScoreOption = 0;        // 0 = 5, 1 = 11, 2 = 15, 3 = 21
    int maxScore = 5;
    bool sfxEnabled = true;
    int spaceThemeOption = 0;      // 0 = Default, 1 = Galaxy, 2 = Trap
    int ballThemeOption = 0;       // 0 = Default, 1 = Banana, 2 = Beach, 3 = Cloudy, 4 = Simple
    int paddleThemeOption = 0;     // 0 = Default, 1 = Cloudy, 2 = CPU, 3 = Human
    int selectedSettingLine = 0;   // 0 = Resolution, 1 = Framerate, 2 = Screen Mode, 3 = Score Limit, 4 = Sound, 5 = Space, 6 = Ball, 7 = Paddle, 8 = Back
};

struct GameContext {
    GameState currentState = GameState::MainMenu;
    float sessionPlayTime = 0.0f;
    bool isPaused = false;
    bool shouldQuit = false;
    bool isMultiplayer = false;
    bool p1Ready = false;
    bool p2Ready = false;
    ScoreBoard score;
    GameConfig config;

    // Asset textures and sound handles
    Texture2D courtBackground = { 0 };
    Texture2D wallsTexture = { 0 };
    Texture2D lineTexture = { 0 };
    Sound paddleHitSound = { 0 };
    Sound wallHitSound = { 0 };
    Sound scoreSound = { 0 };
};

// Asset loading helpers
Texture2D LoadTextureFromResource(int id);
Sound LoadSoundFromResource(int id);
void SetWindowIconFromResource(int id);
// Forward declarations
class Ball;
class Paddle;
class CpuPaddle;

void ApplyPaddleTextures(const GameContext& ctx, Paddle& player, CpuPaddle& cpu);

// Game state update and draw routines
void ResetBall(Ball& ball);
void DrawCourt(const GameContext& ctx, int screenWidth, int screenHeight);
void UpdatePlayingState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu);
void DrawPlayingState(const GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu, int screenWidth, int screenHeight);
void UpdateMultiplayerState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu);
void DrawMultiplayerState(const GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu, int screenWidth, int screenHeight);
void UpdateGameOverState(GameContext& ctx);
void DrawGameOverState(const GameContext& ctx, int screenWidth, int screenHeight);


// Base entity representation
class GameObject {
public:
    Vector2 position;
    Color color;
    GameObject(Vector2 pos, Color c) : position(pos), color(c) {}
    virtual ~GameObject() = default;

    virtual bool Update() = 0;
    virtual void Draw() = 0;
};

// Entity classes
class Paddle : public GameObject {
public:
    float width;
    float height;
    Texture2D texture;
    Paddle(Vector2 pos, Color c, float w, float h, int textureId = 0)
        : GameObject(pos, c), width(w), height(h) {
        if (textureId > 0) {
            texture = LoadTextureFromResource(textureId);
        } else {
            texture = { 0 };
        }
    }

    ~Paddle() override {
        if (texture.id > 0) {
            UnloadTexture(texture);
        }
    }

    bool Update() override;
    void Draw() override;
    void ReloadTexture(int resourceId);
};

class Ball : public GameObject {
public:
    float radius;
    Vector2 velocity;
    Texture2D texture;

public:
    Ball(Vector2 pos, Color c, float r, int textureId = 0)
        : GameObject(pos, c), radius(r), velocity({ 5.0f, 5.0f }) {
        if (textureId > 0) {
            texture = LoadTextureFromResource(textureId);
        } else {
            texture = { 0 };
        }
    }

    ~Ball() override {
        if (texture.id > 0) {
            UnloadTexture(texture);
        }
    }

    bool Update() override;
    void Draw() override;
    void ReloadTexture(int resourceId);
};

class CpuPaddle : public Paddle {
private:
    float speed;
    Difficulty currentDifficulty;

public:
    CpuPaddle(Vector2 pos, Color c, float w, float h, Difficulty diff = Difficulty::Normal, int textureId = 0)
        : Paddle(pos, c, w, h, textureId) {
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
        float dt = GetFrameTime();
        float moveAmount = speed * dt;

        if (paddleCenter > ball_y + moveAmount) {
            position.y -= moveAmount;
        }
        else if (paddleCenter < ball_y - moveAmount) {
            position.y += moveAmount;
        }

        LimitMovement();
        position.x = 20.0f + 10.0f;
    }

    bool Update() override { return false; }

    void LimitMovement() {
        if (position.y <= 20.0f) {
            position.y = 20.0f;
        }
        if (position.y + height >= GetScreenHeight() - 20.0f) {
            position.y = GetScreenHeight() - 20.0f - height;
        }
    }
};