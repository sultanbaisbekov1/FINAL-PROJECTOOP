#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "globals.h"
#include <vector>

class Level;
class Enemy;

class Player {
public:
    Player();
    ~Player() = default;

    void spawn(Level* level);
    void kill();
    void moveHorizontally(float delta, Level* level);
    void jump();
    void update(Level* level, std::vector<Enemy*>& enemies, Sound coinSound, Sound exitSound, Sound killEnemySound, Sound playerDeathSound, size_t gameFrame);
    void updateGravity(Level* level);
    void updateTimer(int delta) { timer = std::max(0, timer + delta); }

    Vector2 getPosition() const { return position; }
    bool isOnGround() const { return onGround; }
    bool isLookingForward() const { return lookingForward; }
    bool isMoving() const { return moving; }
    bool isDead() const { return dead; }
    int getLives() const { return lives; }
    int getTimer() const { return timer; }
    int getTotalScore() const;

    void resetStats();
    void incrementScore();

private:
    Vector2 position;
    float yVelocity;
    bool onGround;
    bool lookingForward;
    bool moving;
    bool dead;
    int lives;
    int timer;
    int timeToCoinCounter;
    int levelScores[LEVEL_COUNT];
};

#endif // PLAYER_H