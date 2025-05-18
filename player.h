#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "globals.h"
#include <vector>
#include <cstddef>

class Level;
class Enemy;

class Player {
public:
    Player();

    void resetStats();
    void incrementScore();
    int getTotalScore() const;
    void spawn(Level* level);
    void kill();
    void moveHorizontally(float delta, Level* level);
    void jump();
    void update(Level* level, std::vector<Enemy*>& enemies, Sound coinSound, Sound exitSound, Sound killEnemySound, Sound playerDeathSound, size_t gameFrame);
    void updateGravity(Level* level);

    Vector2 getPosition() const { return position; }
    bool isOnGround() const { return onGround; }
    bool isLookingForward() const { return lookingForward; }
    bool isMoving() const { return moving; }
    int getLives() const { return lives; }
    bool isDead() const { return dead; }
    int getTimer() const { return timer; }

private:
    Vector2 position;
    float yVelocity;
    bool onGround;
    bool lookingForward;
    bool moving;
    bool dead;
    int lives;
    int levelScores[LEVEL_COUNT];
    int timer;
    int timeToCoinCounter;
};

#endif // PLAYER_H