#include "enemy.h"
#include "level.h"
#include <cmath>

Enemy::Enemy(Vector2 pos, bool lookingRight)
    : position(pos)
    , lookingRight(lookingRight) {}

void Enemy::update(Level* level) {
    float nextX = position.x + (lookingRight ? MOVEMENT_SPEED : -MOVEMENT_SPEED);

    if (level->isColliding({nextX, position.y}, level->getWallChar())) {
        lookingRight = !lookingRight;
    }
    else {
        position.x = nextX;
    }
}

Vector2 Enemy::getPosition() const {
    return position;
}

bool Enemy::isLookingRight() const {
    return lookingRight;
}