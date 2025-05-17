#ifndef PLATFORMER_H
#define PLATFORMER_H

#include "raylib.h"
#include <vector>
#include <string>
#include <cstddef>

class Level;
class Player;
class Enemy;
class Graphics;

class Game {
public:
    enum GameState {
        MENU_STATE,
        GAME_STATE,
        PAUSED_STATE,
        DEATH_STATE,
        GAME_OVER_STATE,
        VICTORY_STATE
    };

    Game();
    ~Game();

    void run();
    void update();
    void draw();

private:
    void loadAssets();
    void unloadAssets();

    GameState gameState;
    size_t gameFrame;
    int levelIndex;
    static const int LEVEL_COUNT = 3;

    Level* currentLevel;
    Player* player;
    std::vector<Enemy*> enemies;
    Graphics* graphics;

    Font menuFont;
    Sound coinSound;
    Sound exitSound;
    Sound killEnemySound;
    Sound playerDeathSound;
    Sound gameOverSound;
};

#endif // PLATFORMER_H