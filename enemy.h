#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <cstddef>

class Level;

class Enemy {
public:
    Enemy(Vector2 pos, bool lookingRight = true);

    void update(Level* level);
    Vector2 getPosition() const;
    bool isLookingRight() const;

private:
    static constexpr float MOVEMENT_SPEED = 0.07f;

    Vector2 position;
    bool lookingRight;
};

#endif // ENEMY_H