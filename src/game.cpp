#include "game.h"
#include "main.h"

// Paddle implementation
bool Paddle::Update() {
    float dt = GetFrameTime();

    if (IsKeyDown(KEY_UP)) {
        position.y -= PLAYER_SPEED * dt;
    }
    if (IsKeyDown(KEY_DOWN)) {
        position.y += PLAYER_SPEED * dt;
    }

    // Keep paddle within screen boundaries
    if (position.y <= 20.0f) {
        position.y = 20.0f;
    }
    if (position.y + height >= GetScreenHeight() - 20.0f) {
        position.y = GetScreenHeight() - 20.0f - height;
    }

    // Align paddle relative to right screen edge
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


// Ball implementation
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
        DrawCircle((int)position.x, (int)position.y, radius, color);
    }
}


// Game state routines and helpers
void ResetBall(Ball& ball) {
    ball.position.x = GetScreenWidth() / 2.0f;
    ball.position.y = GetScreenHeight() / 2.0f;

    int speed_choices[2] = { -1, 1 };
    ball.velocity.x = BALL_SPEED * speed_choices[GetRandomValue(0, 1)];
    ball.velocity.y = BALL_SPEED * speed_choices[GetRandomValue(0, 1)];
}

void DrawCourt(const GameContext& ctx, int screenWidth, int screenHeight) {
    // Left court field background
    DrawTexturePro(
        ctx.courtBackground,
        Rectangle{ 0.0f, 0.0f, (float)ctx.courtBackground.width, (float)ctx.courtBackground.height },
        Rectangle{ 20.0f, 20.0f, (float)screenWidth / 2.0f - 70.0f, (float)screenHeight - 40.0f },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    // Right court field background
    DrawTexturePro(
        ctx.courtBackground,
        Rectangle{ 0.0f, 0.0f, (float)ctx.courtBackground.width, (float)ctx.courtBackground.height },
        Rectangle{ (float)screenWidth / 2.0f + 50.0f, 20.0f, (float)screenWidth / 2.0f - 70.0f, (float)screenHeight - 40.0f },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    // Center circle decoration
    DrawCircle(screenWidth / 2, screenHeight / 2, 50.0f, Color{ 102, 51, 153, 100 });
    DrawCircleLines(screenWidth / 2, screenHeight / 2, 50.0f, Color{ 50, 25, 75, 250 });

    // Center dotted divider
    int lineY = 20;
    while (lineY < screenHeight - 20) {
        DrawTexture(ctx.lineTexture, screenWidth / 2 - ctx.lineTexture.width / 2, lineY, WHITE);
        lineY += ctx.lineTexture.height;
    }

    // Solid background for walls to prevent transparent edge bleeding
    Color wallBg = Color{ 72, 85, 107, 255 };
    DrawRectangle(0, 0, screenWidth, 20, wallBg);
    DrawRectangle(0, screenHeight - 20, screenWidth, 20, wallBg);
    DrawRectangle(0, 0, 20, screenHeight, wallBg);
    DrawRectangle(screenWidth - 20, 0, 20, screenHeight, wallBg);

    // Border walls (top, bottom, left, right)
    DrawTexturePro(ctx.wallsTexture, Rectangle{ 0.0f, 0.0f, (float)ctx.wallsTexture.width, (float)ctx.wallsTexture.height }, Rectangle{ 0.0f, 0.0f, (float)screenWidth, 20.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawTexturePro(ctx.wallsTexture, Rectangle{ 0.0f, 0.0f, (float)ctx.wallsTexture.width, (float)ctx.wallsTexture.height }, Rectangle{ 0.0f, (float)screenHeight - 20.0f, (float)screenWidth, 20.0f }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawTexturePro(ctx.wallsTexture, Rectangle{ 0.0f, 0.0f, (float)ctx.wallsTexture.width, (float)ctx.wallsTexture.height }, Rectangle{ 0.0f, 0.0f, 20.0f, (float)screenHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawTexturePro(ctx.wallsTexture, Rectangle{ 0.0f, 0.0f, (float)ctx.wallsTexture.width, (float)ctx.wallsTexture.height }, Rectangle{ (float)screenWidth - 20.0f, 0.0f, 20.0f, (float)screenHeight }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
}

void UpdatePlayingState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu) {
    if (IsKeyPressed(KEY_P)) {
        ctx.isPaused = !ctx.isPaused;
    }
    if (IsKeyPressed(KEY_SPACE)) {
        ctx.currentState = GameState::GameOver;
    }

    if (!ctx.isPaused) {
        ctx.sessionPlayTime += GetFrameTime();

        if (ball.Update() && ctx.config.sfxEnabled) {
            PlaySound(ctx.wallHitSound);
        }
        player.Update();
        cpu.Update(ball.position.y);

        // Paddle collisions
        if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
            ball.velocity.x *= -1;
            if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
        }
        if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
            ball.velocity.x *= -1;
            if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
        }

        // Scoring rules
        int screen_width = GetScreenWidth();
        if (ball.position.x + ball.radius >= screen_width - 20.0f) {
            ctx.score.cpu_score++;
            if (ctx.config.sfxEnabled) PlaySound(ctx.scoreSound);
            if (ctx.score.cpu_score >= ctx.config.maxScore) {
                ctx.currentState = GameState::GameOver;
            } else {
                ResetBall(ball);
            }
        }
        if (ball.position.x - ball.radius <= 20.0f) {
            ctx.score.player_score++;
            if (ctx.config.sfxEnabled) PlaySound(ctx.scoreSound);
            if (ctx.score.player_score >= ctx.config.maxScore) {
                ctx.currentState = GameState::GameOver;
            } else {
                ResetBall(ball);
            }
        }
    }
} 

void DrawPlayingState(const GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu, int screenWidth, int screenHeight) {
    DrawCourt(ctx, screenWidth, screenHeight);

    // Header scores
    DrawText(TextFormat("%i", ctx.score.cpu_score), screenWidth / 4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%i", ctx.score.player_score), 3 * screenWidth / 4 - 20, 20, 80, WHITE);

    // Playtime tracker display
    int minutes = (int)ctx.sessionPlayTime / 60;
    int seconds = (int)ctx.sessionPlayTime % 60;
    int timeTextWidth = MeasureText(TextFormat("%02i:%02i", minutes, seconds), 32);
    
    DrawRectangle(screenWidth / 2 - timeTextWidth / 2 - 15, screenHeight - 85, timeTextWidth + 30, 44, Color{ 15, 15, 15, 220 });
    DrawRectangleLines(screenWidth / 2 - timeTextWidth / 2 - 15, screenHeight - 85, timeTextWidth + 30, 44, Color{ 100, 100, 100, 255 });
    DrawText(TextFormat("%02i:%02i", minutes, seconds), screenWidth / 2 - timeTextWidth / 2, screenHeight - 79, 32, YELLOW);

    ball.Draw();
    cpu.Draw();
    player.Draw();

    // Pause interface overlay
    if (ctx.isPaused) {
        DrawRectangle(20, 20, screenWidth - 40, screenHeight - 40, Color{ 0, 0, 0, 150 });
        int pausedTextWidth = MeasureText("PAUSED", 60);
        DrawText("PAUSED", screenWidth / 2 - pausedTextWidth / 2, screenHeight / 2 - 60, 60, YELLOW);
        int hintTextWidth = MeasureText("UP/DOWN Arrows to move | SPACE: Force Game Over", 20);
        DrawText("UP/DOWN Arrows to move | SPACE: Force Game Over", screenWidth / 2 - hintTextWidth / 2, screenHeight / 2 + 20, 20, WHITE);
    }
}

void UpdateMultiplayerState(GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu) {
    if (IsKeyPressed(KEY_P)) {
        ctx.isPaused = !ctx.isPaused;
    }
    if (IsKeyPressed(KEY_SPACE)) {
        ctx.currentState = GameState::GameOver;
    }

    if (!ctx.isPaused) {
        ctx.sessionPlayTime += GetFrameTime();

        if (ball.Update() && ctx.config.sfxEnabled) {
            PlaySound(ctx.wallHitSound);
        }
        player.Update();

        // Player 2 controls (Left Paddle)
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_W)) {
            cpu.position.y -= PLAYER_SPEED * dt;
        }
        if (IsKeyDown(KEY_S)) {
            cpu.position.y += PLAYER_SPEED * dt;
        }
        cpu.LimitMovement();
        cpu.position.x = 20.0f + 10.0f;

        // Paddle collisions
        if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ player.position.x, player.position.y, player.width, player.height })) {
            ball.velocity.x *= -1;
            if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
        }
        if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{ cpu.position.x, cpu.position.y, cpu.width, cpu.height })) {
            ball.velocity.x *= -1;
            if (ctx.config.sfxEnabled) PlaySound(ctx.paddleHitSound);
        }

        // Scoring rules
        int screen_width = GetScreenWidth();
        if (ball.position.x + ball.radius >= screen_width - 30.0f) {
            ctx.score.player2_score++;
            if (ctx.config.sfxEnabled) PlaySound(ctx.scoreSound);
            if (ctx.score.player2_score >= ctx.config.maxScore) {
                ctx.currentState = GameState::GameOver;
            } else {
                ResetBall(ball);
            }
        }
        if (ball.position.x - ball.radius <= 20.0f) {
            ctx.score.player_score++;
            if (ctx.config.sfxEnabled) PlaySound(ctx.scoreSound);
            if (ctx.score.player_score >= ctx.config.maxScore) {
                ctx.currentState = GameState::GameOver;
            } else {
                ResetBall(ball);
            }
        }
    }
}

void DrawMultiplayerState(const GameContext& ctx, Ball& ball, Paddle& player, CpuPaddle& cpu, int screenWidth, int screenHeight) {
    DrawCourt(ctx, screenWidth, screenHeight);

    // Header scores
    DrawText(TextFormat("%i", ctx.score.player2_score), screenWidth / 4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%i", ctx.score.player_score), 3 * screenWidth / 4 - 20, 20, 80, WHITE);

    // Playtime tracker display
    int minutes = (int)ctx.sessionPlayTime / 60;
    int seconds = (int)ctx.sessionPlayTime % 60;
    int timeTextWidth = MeasureText(TextFormat("%02i:%02i", minutes, seconds), 32);
    
    DrawRectangle(screenWidth / 2 - timeTextWidth / 2 - 15, screenHeight - 85, timeTextWidth + 30, 44, Color{ 15, 15, 15, 220 });
    DrawRectangleLines(screenWidth / 2 - timeTextWidth / 2 - 15, screenHeight - 85, timeTextWidth + 30, 44, Color{ 100, 100, 100, 255 });
    DrawText(TextFormat("%02i:%02i", minutes, seconds), screenWidth / 2 - timeTextWidth / 2, screenHeight - 79, 32, YELLOW);

    ball.Draw();
    cpu.Draw();
    player.Draw();

    // Pause interface overlay
    if (ctx.isPaused) {
        DrawRectangle(20, 20, screenWidth - 40, screenHeight - 40, Color{ 0, 0, 0, 150 });
        int pausedTextWidth = MeasureText("PAUSED", 60);
        DrawText("PAUSED", screenWidth / 2 - pausedTextWidth / 2, screenHeight / 2 - 60, 60, YELLOW);
        int hintTextWidth = MeasureText("P1 (Right): UP/DOWN | P2 (Left): W/S | SPACE: Force Game Over", 20);
        DrawText("P1 (Right): UP/DOWN | P2 (Left): W/S | SPACE: Force Game Over", screenWidth / 2 - hintTextWidth / 2, screenHeight / 2 + 20, 20, WHITE);
    }
}

void UpdateGameOverState(GameContext& ctx) {
    if (IsKeyPressed(KEY_SPACE)) {
        ctx.currentState = GameState::MainMenu;
    }
}

void DrawGameOverState(const GameContext& ctx, int screenWidth, int screenHeight) {
    int panelWidth = 600;
    int panelHeight = 400;
    int panelX = screenWidth / 2 - panelWidth / 2;
    int panelY = screenHeight / 2 - panelHeight / 2;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{ 15, 15, 15, 230 });
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{ 120, 120, 120, 255 });

    int gameOverWidth = MeasureText("GAME OVER", 60);
    DrawText("GAME OVER", screenWidth / 2 - gameOverWidth / 2, panelY + 40, 60, RED);

    std::string winnerText = "GAME ENDED";
    Color winnerColor = YELLOW;
    std::string score1Str, score2Str;

    if (ctx.isMultiplayer) {
        if (ctx.score.player_score > ctx.score.player2_score) {
            winnerText = "PLAYER 1 WINS!";
            winnerColor = Green;
        }
        else if (ctx.score.player2_score > ctx.score.player_score) {
            winnerText = "PLAYER 2 WINS!";
            winnerColor = RED;
        }
        score1Str = "Player 1 Score: " + std::to_string(ctx.score.player_score);
        score2Str = "Player 2 Score: " + std::to_string(ctx.score.player2_score);
    }
    else {
        if (ctx.score.player_score > ctx.score.cpu_score) {
            winnerText = "YOU WIN!";
            winnerColor = Green;
        }
        else if (ctx.score.cpu_score > ctx.score.player_score) {
            winnerText = "CPU WINS!";
            winnerColor = RED;
        }
        score1Str = "Player Score: " + std::to_string(ctx.score.player_score);
        score2Str = "CPU Score: " + std::to_string(ctx.score.cpu_score);
    }

    int winnerWidth = MeasureText(winnerText.c_str(), 40);
    DrawText(winnerText.c_str(), screenWidth / 2 - winnerWidth / 2, panelY + 120, 40, winnerColor);

    int playerTextWidth = MeasureText(score1Str.c_str(), 30);
    int cpuTextWidth = MeasureText(score2Str.c_str(), 30);
    DrawText(score1Str.c_str(), screenWidth / 2 - playerTextWidth / 2, panelY + 200, 30, WHITE);
    DrawText(score2Str.c_str(), screenWidth / 2 - cpuTextWidth / 2, panelY + 250, 30, WHITE);

    int minutes = (int)ctx.sessionPlayTime / 60;
    int seconds = (int)ctx.sessionPlayTime % 60;
    std::string timeStr = "Playtime: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    int timeTextWidth = MeasureText(timeStr.c_str(), 20);
    DrawText(timeStr.c_str(), screenWidth / 2 - timeTextWidth / 2, panelY + 300, 20, LIGHTGRAY);

    int instWidth = MeasureText("Press SPACEBAR to return to Main Menu", 20);
    DrawText("Press SPACEBAR to return to Main Menu", screenWidth / 2 - instWidth / 2, panelY + 340, 20, YELLOW);
}
