#include "graphics.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

const Color Graphics::VICTORY_BALL_COLOR = {180, 180, 180, 255};

Graphics::Graphics(Player* player) : player(player), screenScale(1.0f), cellSize(0), horizontalShift(0) {
    screenSize = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    menuFont = LoadFontEx("data/fonts/ARCADE_N.TTF", 256, nullptr, 128);
    gameTitle = {"Platformer", {0.50f, 0.50f}, 100.0f, RED, 4.0f, &menuFont};
    gameSubtitle = {"Press Enter to Start", {0.50f, 0.65f}, 32.0f, WHITE, 4.0f, &menuFont};
    gamePaused = {"Press Escape to Resume", {0.50f, 0.50f}, 32.0f, WHITE, 4.0f, &menuFont};
    deathTitle = {"You Died!", {0.50f, 0.50f}, 80.0f, RED, 4.0f, &menuFont};
    deathSubtitle = {"Press Enter to Try Again", {0.50f, 0.65f}, 32.0f, WHITE, 4.0f, &menuFont};
    gameOverTitle = {"Game Over", {0.50f, 0.50f}, 120.0f, RED, 4.0f, &menuFont};
    gameOverSubtitle = {"Press Enter to Restart", {0.50f, 0.675f}, 32.0f, WHITE, 4.0f, &menuFont};
    victoryTitle = {"You Won!", {0.50f, 0.50f}, 100.0f, RED, 4.0f, &menuFont};
    victorySubtitle = {"Press Enter to go back to menu", {0.50f, 0.65f}, 32.0f, WHITE, 4.0f, &menuFont};

    loadAssets();
}

Graphics::~Graphics() {
    unloadAssets();
}

void Graphics::loadAssets() {
    wallImage = LoadTexture("data/images/wall.png");
    wallDarkImage = LoadTexture("data/images/wall_dark.png");
    spikeImage = LoadTexture("data/images/spikes.png");
    exitImage = LoadTexture("data/images/exit.png");

    coinSprite = Sprite{3, 18, 0, 0, true, 0, new Texture2D[3]};
    coinSprite.frames[0] = LoadTexture("data/images/coin/coin0.png");
    coinSprite.frames[1] = LoadTexture("data/images/coin/coin1.png");
    coinSprite.frames[2] = LoadTexture("data/images/coin/coin2.png");

    heartImage = LoadTexture("data/images/heart.png");

    playerStandForwardImage = LoadTexture("data/images/player_stand_forward.png");
    playerStandBackwardsImage = LoadTexture("data/images/player_stand_backwards.png");
    playerJumpForwardImage = LoadTexture("data/images/player_jump_forward.png");
    playerJumpBackwardsImage = LoadTexture("data/images/player_jump_backwards.png");
    playerDeadImage = LoadTexture("data/images/player_dead.png");

    playerWalkForwardSprite = Sprite{3, 15, 0, 0, true, 0, new Texture2D[3]};
    playerWalkForwardSprite.frames[0] = LoadTexture("data/images/player_walk_forward/player0.png");
    playerWalkForwardSprite.frames[1] = LoadTexture("data/images/player_walk_forward/player1.png");
    playerWalkForwardSprite.frames[2] = LoadTexture("data/images/player_walk_forward/player2.png");

    playerWalkBackwardsSprite = Sprite{3, 15, 0, 0, true, 0, new Texture2D[3]};
    playerWalkBackwardsSprite.frames[0] = LoadTexture("data/images/player_walk_backwards/player0.png");
    playerWalkBackwardsSprite.frames[1] = LoadTexture("data/images/player_walk_backwards/player1.png");
    playerWalkBackwardsSprite.frames[2] = LoadTexture("data/images/player_walk_backwards/player2.png");

    enemyWalkSprite = Sprite{2, 15, 0, 0, true, 0, new Texture2D[2]};
    enemyWalkSprite.frames[0] = LoadTexture("data/images/enemy_walk/enemy0.png");
    enemyWalkSprite.frames[1] = LoadTexture("data/images/enemy_walk/enemy1.png");

    backgroundImage = LoadTexture("data/images/background/background.png");
    middlegroundImage = LoadTexture("data/images/background/middleground.png");
    foregroundImage = LoadTexture("data/images/background/foreground.png");
}

void Graphics::unloadAssets() {
    UnloadFont(menuFont);
    UnloadTexture(wallImage);
    UnloadTexture(wallDarkImage);
    UnloadTexture(spikeImage);
    UnloadTexture(exitImage);

    for (size_t i = 0; i < coinSprite.frameCount; ++i) {
        UnloadTexture(coinSprite.frames[i]);
    }
    delete[] coinSprite.frames;

    UnloadTexture(heartImage);
    UnloadTexture(playerStandForwardImage);
    UnloadTexture(playerStandBackwardsImage);
    UnloadTexture(playerJumpForwardImage);
    UnloadTexture(playerJumpBackwardsImage);
    UnloadTexture(playerDeadImage);

    for (size_t i = 0; i < playerWalkForwardSprite.frameCount; ++i) {
        UnloadTexture(playerWalkForwardSprite.frames[i]);
    }
    delete[] playerWalkForwardSprite.frames;

    for (size_t i = 0; i < playerWalkBackwardsSprite.frameCount; ++i) {
        UnloadTexture(playerWalkBackwardsSprite.frames[i]);
    }
    delete[] playerWalkBackwardsSprite.frames;

    for (size_t i = 0; i < enemyWalkSprite.frameCount; ++i) {
        UnloadTexture(enemyWalkSprite.frames[i]);
    }
    delete[] enemyWalkSprite.frames;

    UnloadTexture(backgroundImage);
    UnloadTexture(middlegroundImage);
    UnloadTexture(foregroundImage);
}

void Graphics::drawText(const Text& text) {
    Vector2 dimensions = MeasureTextEx(*text.font, text.str.c_str(), text.size * screenScale, text.spacing);
    Vector2 pos = {
        (screenSize.x * text.position.x) - (0.5f * dimensions.x),
        (screenSize.y * text.position.y) - (0.5f * dimensions.y)
    };
    DrawTextEx(*text.font, text.str.c_str(), pos, dimensions.y, text.spacing, text.color);
}

void Graphics::drawSprite(Sprite& sprite, Vector2 pos, float size, size_t gameFrame) {
    if (gameFrame != sprite.prevGameFrame) {
        sprite.framesSkipped++;
        if (sprite.framesSkipped >= sprite.framesToSkip) {
            sprite.framesSkipped = 0;
            sprite.frameIndex = (sprite.frameIndex + 1) % (sprite.loop ? sprite.frameCount : sprite.frameCount);
        }
        sprite.prevGameFrame = gameFrame;
    }

    Rectangle source = {0.0f, 0.0f, static_cast<float>(sprite.frames[sprite.frameIndex].width), static_cast<float>(sprite.frames[sprite.frameIndex].height)};
    Rectangle destination = {pos.x, pos.y, size, size};
    DrawTexturePro(sprite.frames[sprite.frameIndex], source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}

void Graphics::drawImage(Texture2D image, Vector2 pos, float size) {
    Rectangle source = {0.0f, 0.0f, static_cast<float>(image.width), static_cast<float>(image.height)};
    Rectangle destination = {pos.x, pos.y, size, size};
    DrawTexturePro(image, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}

void Graphics::deriveMetricsFromLevel(Level* level) {
    screenSize.x = static_cast<float>(GetScreenWidth());
    screenSize.y = static_cast<float>(GetScreenHeight());
    cellSize = screenSize.y / static_cast<float>(level->getRows());
    screenScale = std::min(screenSize.x, screenSize.y) / SCREEN_SCALE_DIVISOR;

    float largerScreenSide = std::max(screenSize.x, screenSize.y);
    if (screenSize.x > screenSize.y) {
        backgroundSize = {largerScreenSide, largerScreenSide / 16 * 10};
    } else {
        backgroundSize = {largerScreenSide / 10 * 16, largerScreenSide};
    }
    backgroundYOffset = (screenSize.y - backgroundSize.y) * 0.5f;
}

void Graphics::drawParallaxBackground(size_t gameFrame) {
    float initialOffset = -(player->getPosition().x * PARALLAX_PLAYER_SCROLLING_SPEED + gameFrame * PARALLAX_IDLE_SCROLLING_SPEED);
    float backgroundOffset = initialOffset;
    float middlegroundOffset = backgroundOffset * PARALLAX_LAYERED_SPEED_DIFFERENCE;
    float foregroundOffset = middlegroundOffset * PARALLAX_LAYERED_SPEED_DIFFERENCE;

    backgroundOffset = fmod(backgroundOffset, 1.0f);
    middlegroundOffset = fmod(middlegroundOffset, 1.0f);
    foregroundOffset = fmod(foregroundOffset, 1.0f);

    backgroundOffset *= backgroundSize.x;
    middlegroundOffset *= backgroundSize.x;
    foregroundOffset *= backgroundSize.x;

    Rectangle source = {0.0f, 0.0f, static_cast<float>(backgroundImage.width), static_cast<float>(backgroundImage.height)};
    Rectangle dest1 = {backgroundOffset + backgroundSize.x, backgroundYOffset, backgroundSize.x, backgroundSize.y};
    Rectangle dest2 = {backgroundOffset, backgroundYOffset, backgroundSize.x, backgroundSize.y};
    DrawTexturePro(backgroundImage, source, dest1, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTexturePro(backgroundImage, source, dest2, {0.0f, 0.0f}, 0.0f, WHITE);

    dest1.x = middlegroundOffset + backgroundSize.x;
    dest2.x = middlegroundOffset;
    DrawTexturePro(middlegroundImage, source, dest1, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTexturePro(middlegroundImage, source, dest2, {0.0f, 0.0f}, 0.0f, WHITE);

    dest1.x = foregroundOffset + backgroundSize.x;
    dest2.x = foregroundOffset;
    DrawTexturePro(foregroundImage, source, dest1, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTexturePro(foregroundImage, source, dest2, {0.0f, 0.0f}, 0.0f, WHITE);
}

void Graphics::drawMenu() {
    ClearBackground(BLACK);
    drawText(gameTitle);
    drawText(gameSubtitle);
}

void Graphics::drawGame(Level* level, const std::vector<Enemy*>& enemies, size_t gameFrame) {
    ClearBackground(BLACK);
    deriveMetricsFromLevel(level);
    drawParallaxBackground(gameFrame);

    horizontalShift = (screenSize.x - cellSize) / 2;

    for (size_t row = 0; row < level->getRows(); ++row) {
        for (size_t column = 0; column < level->getColumns(); ++column) {
            Vector2 pos = {
                (static_cast<float>(column) - player->getPosition().x) * cellSize + horizontalShift,
                static_cast<float>(row) * cellSize
            };

            char cell = level->getCell(row, column);
            if (cell == level->getWallChar()) {
                drawImage(wallImage, pos, cellSize);
            } else if (cell == level->getWallDarkChar()) {
                drawImage(wallDarkImage, pos, cellSize);
            } else if (cell == level->getSpikeChar()) {
                drawImage(spikeImage, pos, cellSize);
            } else if (cell == level->getCoinChar()) {
                drawSprite(coinSprite, pos, cellSize, gameFrame);
            } else if (cell == level->getExitChar()) {
                drawImage(exitImage, pos, cellSize);
            }
        }
    }

    for (const auto& enemy : enemies) {
        Vector2 pos = {
            (enemy->getPosition().x - player->getPosition().x) * cellSize + horizontalShift,
            enemy->getPosition().y * cellSize
        };
        drawSprite(enemyWalkSprite, pos, cellSize, gameFrame);
    }

    Vector2 playerPos = {horizontalShift, player->getPosition().y * cellSize};
    if (!player->isDead()) {
        if (!player->isOnGround()) {
            drawImage(player->isLookingForward() ? playerJumpForwardImage : playerJumpBackwardsImage, playerPos, cellSize);
        } else if (player->isMoving()) {
            drawSprite(player->isLookingForward() ? playerWalkForwardSprite : playerWalkBackwardsSprite, playerPos, cellSize, gameFrame);
        } else {
            drawImage(player->isLookingForward() ? playerStandForwardImage : playerStandBackwardsImage, playerPos, cellSize);
        }
    } else {
        drawImage(playerDeadImage, playerPos, cellSize);
    }

    const float ICON_SIZE = 48.0f * screenScale;
    float verticalOffset = 8.0f * screenScale;

    for (int i = 0; i < player->getLives(); i++) {
        Vector2 heartPos = {ICON_SIZE * i + (4.0f * screenScale), verticalOffset};
        drawImage(heartImage, heartPos, ICON_SIZE);
    }

    Vector2 timerDimensions = MeasureTextEx(menuFont, std::to_string(player->getTimer() / 60).c_str(), ICON_SIZE, 2.0f);
    Vector2 timerPosition = {(GetRenderWidth() - timerDimensions.x) * 0.5f, verticalOffset};
    DrawTextEx(menuFont, std::to_string(player->getTimer() / 60).c_str(), timerPosition, ICON_SIZE, 2.0f, WHITE);

    Vector2 scoreDimensions = MeasureTextEx(menuFont, std::to_string(player->getTotalScore()).c_str(), ICON_SIZE, 2.0f);
    Vector2 scorePosition = {GetRenderWidth() - scoreDimensions.x - ICON_SIZE, verticalOffset};
    DrawTextEx(menuFont, std::to_string(player->getTotalScore()).c_str(), scorePosition, ICON_SIZE, 2.0f, WHITE);
    drawSprite(coinSprite, {GetRenderWidth() - ICON_SIZE, verticalOffset}, ICON_SIZE, gameFrame);
}

void Graphics::drawDeathScreen(Level* level, const std::vector<Enemy*>& enemies, size_t gameFrame) {
    drawGame(level, enemies, gameFrame);
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), {0, 0, 0, 100});
    drawText(deathTitle);
    drawText(deathSubtitle);
}

void Graphics::drawGameOverMenu() {
    ClearBackground(BLACK);
    drawText(gameOverTitle);
    drawText(gameOverSubtitle);
}

void Graphics::drawPauseMenu() {
    ClearBackground(BLACK);
    drawText(gamePaused);
}

void Graphics::drawVictoryMenu(size_t gameFrame) {
    DrawRectangle(0, 0, static_cast<int>(screenSize.x), static_cast<int>(screenSize.y), {0, 0, 0, VICTORY_BALL_TRAIL_TRANSPARENCY});

    for (auto& ball : victoryBalls) {
        ball.x += ball.dx;
        if (ball.x - ball.radius < 0 || ball.x + ball.radius >= screenSize.x) {
            ball.dx = -ball.dx;
        }
        ball.y += ball.dy;
        if (ball.y - ball.radius < 0 || ball.y + ball.radius >= screenSize.y) {
            ball.dy = -ball.dy;
        }
        DrawCircleV({ball.x, ball.y}, ball.radius, VICTORY_BALL_COLOR);
    }

    drawText(victoryTitle);
    drawText(victorySubtitle);
}

void Graphics::initializeVictoryBalls() {
    deriveMetricsFromLevel(nullptr);
    for (auto& ball : victoryBalls) {
        ball.x = static_cast<float>(rand()) / RAND_MAX * screenSize.x;
        ball.y = static_cast<float>(rand()) / RAND_MAX * screenSize.y;
        ball.dx = (static_cast<float>(rand()) / RAND_MAX * 2 - 1) * VICTORY_BALL_MAX_SPEED * screenScale;
        if (abs(ball.dx) < 0.1f) ball.dx = 1.0f;
        ball.dy = (static_cast<float>(rand()) / RAND_MAX * 2 - 1) * VICTORY_BALL_MAX_SPEED * screenScale;
        if (abs(ball.dy) < 0.1f) ball.dy = 1.0f;
        ball.radius = (static_cast<float>(rand()) / RAND_MAX * (VICTORY_BALL_MAX_RADIUS - VICTORY_BALL_MIN_RADIUS) + VICTORY_BALL_MIN_RADIUS) * screenScale;
    }
}