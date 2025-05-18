#include "enemy.h"
#include "level.h"
#include <cmath>

Enemy::Enemy(Vector2 pos, bool lookingRight)
    : position(pos)
    , patrolStart(pos)
    , lookingRight(lookingRight)
    , currentState(State::PATROLLING)
    , patrolDistance(3.0f)
    , initialPatrolDistance(3.0f) {}

void Enemy::update(Level* level) {
    switch (currentState) {
        case State::PATROLLING:
            patrol(level);
            break;
        case State::CHASING:
            // Player position will be updated in updateState
            break;
        case State::RETURNING:
            returnToPatrol(level);
            break;
    }
}

void Enemy::updateState(Level* level, const Vector2& playerPos) {
    float distance = distanceToPlayer(playerPos);

    switch (currentState) {
        case State::PATROLLING:
            if (distance < DETECTION_RANGE && canSeePlayer(level, playerPos)) {
                currentState = State::CHASING;
            }
            break;

        case State::CHASING:
            if (distance > CHASE_RANGE || !canSeePlayer(level, playerPos)) {
                currentState = State::RETURNING;
            } else {
                chase(level, playerPos);
            }
            break;

        case State::RETURNING:
            if (distance < DETECTION_RANGE && canSeePlayer(level, playerPos)) {
                currentState = State::CHASING;
            } else if (std::abs(position.x - patrolStart.x) < 0.1f) {
                currentState = State::PATROLLING;
                patrolDistance = initialPatrolDistance;
            }
            break;
    }
}

void Enemy::patrol(Level* level) {
    float nextX = position.x + (lookingRight ? MOVEMENT_SPEED : -MOVEMENT_SPEED);

    if (level->isColliding({nextX, position.y}, level->getWallChar()) ||
        std::abs(nextX - patrolStart.x) > patrolDistance) {
        lookingRight = !lookingRight;
    } else {
        position.x = nextX;
    }
}

void Enemy::chase(Level* level, const Vector2& playerPos) {
    float directionX = playerPos.x - position.x;
    lookingRight = directionX > 0;

    float nextX = position.x + (lookingRight ? CHASE_SPEED : -CHASE_SPEED);

    if (!level->isColliding({nextX, position.y}, level->getWallChar())) {
        position.x = nextX;
    }
}

void Enemy::returnToPatrol(Level* level) {
    float directionX = patrolStart.x - position.x;
    lookingRight = directionX > 0;

    float nextX = position.x + (lookingRight ? MOVEMENT_SPEED : -MOVEMENT_SPEED);

    if (!level->isColliding({nextX, position.y}, level->getWallChar())) {
        position.x = nextX;
    }
}

float Enemy::distanceToPlayer(const Vector2& playerPos) const {
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool Enemy::canSeePlayer(Level* level, const Vector2& playerPos) const {
    // Simple line of sight check
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance == 0) return true;

    // Check points along the line between enemy and player
    float stepX = dx / distance;
    float stepY = dy / distance;

    Vector2 checkPos = position;
    for (float i = 0; i < distance; i += 0.5f) {
        checkPos.x = position.x + stepX * i;
        checkPos.y = position.y + stepY * i;

        if (level->isColliding(checkPos, level->getWallChar())) {
            return false;
        }
    }

    return true;
}