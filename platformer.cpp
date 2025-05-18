#include "platformer.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include "graphics.h"

Game::Game() : gameState(MENU_STATE), gameFrame(0), levelIndex(0) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1024, 480, "Platformer");
    SetWindowSize(1024, 480);
    SetTargetFPS(60);
    HideCursor();

    currentLevel = new Level();
    player = new Player();
    graphics = new Graphics(player);

    loadAssets();
    currentLevel->load(levelIndex);
    player->spawn(currentLevel);
}

Game::~Game() {
    unloadAssets();
    delete currentLevel;
    delete player;
    delete graphics;
    for (auto enemy : enemies) {
        delete enemy;
    }
    CloseAudioDevice();
    CloseWindow();
}

void Game::loadAssets() {
    menuFont = LoadFontEx("data/fonts/ARCADE_N.TTF", 256, nullptr, 128);
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "Audio device initialization failed. Proceeding without sound.");
    } else {
        coinSound = LoadSound("data/sounds/coin.wav");
        exitSound = LoadSound("data/sounds/exit.wav");
        killEnemySound = LoadSound("data/sounds/kill_enemy.wav");
        playerDeathSound = LoadSound("data/sounds/player_death.wav");
        gameOverSound = LoadSound("data/sounds/game_over.wav");
    }
}

void Game::unloadAssets() {
    UnloadFont(menuFont);
    if (IsAudioDeviceReady()) {
        UnloadSound(coinSound);
        UnloadSound(exitSound);
        UnloadSound(killEnemySound);
        UnloadSound(playerDeathSound);
        UnloadSound(gameOverSound);
    }
}

void Game::update() {
    gameFrame++;
    static GameState previousState = MENU_STATE;

    switch (gameState) {
        case MENU_STATE:
            if (IsAudioDeviceReady()) StopSound(playerDeathSound); // Остановка звука, если он остался
            if (IsKeyPressed(KEY_ENTER)) {
                TraceLog(LOG_INFO, "Transitioning to GAME_STATE");
                gameState = GAME_STATE;
                currentLevel->unload();
                currentLevel->load(levelIndex);
                player->spawn(currentLevel);
                for (auto enemy : enemies) delete enemy;
                enemies.clear();
                for (size_t row = 0; row < currentLevel->getRows(); ++row) {
                    for (size_t column = 0; column < currentLevel->getColumns(); ++column) {
                        if (currentLevel->getCell(row, column) == currentLevel->getEnemyChar()) {
                            enemies.push_back(new Enemy({static_cast<float>(column), static_cast<float>(row)}));
                            currentLevel->setCell(row, column, currentLevel->getAirChar());
                        }
                    }
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                TraceLog(LOG_INFO, "Exiting game from MENU_STATE");
                CloseWindow();
            }
            break;

        case GAME_STATE:
            {
                float deltaX = 0.0f;
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) deltaX += 0.1f;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) deltaX -= 0.1f;
                if (deltaX != 0.0f) player->moveHorizontally(deltaX, currentLevel);

                if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) && player->isOnGround()) {
                    player->jump();
                }

                player->update(currentLevel, enemies, coinSound, exitSound, killEnemySound, playerDeathSound, gameFrame);
                for (auto enemy : enemies) {
                    enemy->update(currentLevel);
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    TraceLog(LOG_INFO, "Transitioning to PAUSED_STATE");
                    gameState = PAUSED_STATE;
                }

                if (currentLevel->isColliding(player->getPosition(), currentLevel->getExitChar()) && player->getTimer() <= 0) {
                    TraceLog(LOG_INFO, "Transitioning to VICTORY_STATE");
                    gameState = VICTORY_STATE;
                }

                if (player->isDead()) {
                    TraceLog(LOG_INFO, "Transitioning to DEATH_STATE");
                    gameState = DEATH_STATE;
                }
            }
            break;

        case PAUSED_STATE:
            if (IsKeyPressed(KEY_ESCAPE)) {
                TraceLog(LOG_INFO, "Returning to GAME_STATE");
                gameState = GAME_STATE;
            }
            break;

        case DEATH_STATE:
            player->updateGravity(currentLevel);
            if (IsKeyPressed(KEY_ENTER)) {
                if (player->getLives() > 0) {
                    TraceLog(LOG_INFO, "Restarting level in GAME_STATE");
                    currentLevel->unload();
                    currentLevel->load(levelIndex);
                    player->spawn(currentLevel);
                    for (auto enemy : enemies) delete enemy;
                    enemies.clear();
                    for (size_t row = 0; row < currentLevel->getRows(); ++row) {
                        for (size_t column = 0; column < currentLevel->getColumns(); ++column) {
                            if (currentLevel->getCell(row, column) == currentLevel->getEnemyChar()) {
                                enemies.push_back(new Enemy({static_cast<float>(column), static_cast<float>(row)}));
                                currentLevel->setCell(row, column, currentLevel->getAirChar());
                            }
                        }
                    }
                    gameState = GAME_STATE;
                } else {
                    TraceLog(LOG_INFO, "Transitioning to GAME_OVER_STATE");
                    currentLevel->unload();
                    gameState = GAME_OVER_STATE;
                    if (IsAudioDeviceReady()) PlaySound(gameOverSound);
                }
                if (IsAudioDeviceReady()) StopSound(playerDeathSound);
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                TraceLog(LOG_INFO, "Returning to MENU_STATE from DEATH_STATE");
                currentLevel->unload();
                gameState = MENU_STATE;
                if (IsAudioDeviceReady()) StopSound(playerDeathSound);
            }
            break;

        case GAME_OVER_STATE:
            if (IsKeyPressed(KEY_ENTER)) {
                TraceLog(LOG_INFO, "Restarting game from GAME_OVER_STATE");
                levelIndex = 0;
                player->resetStats();
                currentLevel->unload();
                currentLevel->load(0);
                player->spawn(currentLevel);
                for (auto enemy : enemies) delete enemy;
                enemies.clear();
                for (size_t row = 0; row < currentLevel->getRows(); ++row) {
                    for (size_t column = 0; column < currentLevel->getColumns(); ++column) {
                        if (currentLevel->getCell(row, column) == currentLevel->getEnemyChar()) {
                            enemies.push_back(new Enemy({static_cast<float>(column), static_cast<float>(row)}));
                            currentLevel->setCell(row, column, currentLevel->getAirChar());
                        }
                    }
                }
                gameState = GAME_STATE;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                TraceLog(LOG_INFO, "Returning to MENU_STATE from GAME_OVER_STATE");
                currentLevel->unload();
                gameState = MENU_STATE;
            }
            break;

        case VICTORY_STATE:
            if (previousState != VICTORY_STATE) {
                graphics->initializeVictoryBalls();
                ClearBackground(BLACK);
                EndDrawing();
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
                BeginDrawing();
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                TraceLog(LOG_INFO, "Returning to MENU_STATE from VICTORY_STATE");
                levelIndex = 0;
                player->resetStats();
                currentLevel->unload();
                gameState = MENU_STATE;
            }
            break;
    }

    previousState = gameState;
}

void Game::draw() {
    BeginDrawing();

    switch(gameState) {
        case MENU_STATE:
            graphics->drawMenu();
            break;
        case GAME_STATE:
            graphics->drawGame(currentLevel, enemies, gameFrame);
            break;
        case DEATH_STATE:
            graphics->drawDeathScreen(currentLevel, enemies, gameFrame);
            break;
        case GAME_OVER_STATE:
            graphics->drawGameOverMenu();
            break;
        case PAUSED_STATE:
            graphics->drawPauseMenu();
            break;
        case VICTORY_STATE:
            graphics->drawVictoryMenu(gameFrame);
            break;
    }

    EndDrawing();
}

void Game::run() {
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

int main() {
    Game game;
    game.run();
    return 0;
}