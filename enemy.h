#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <cstddef>

class Level;

class Enemy {
public:
    enum class State {
        PATROLLING,
        CHASING,
        RETURNING
    };

    Enemy(Vector2 pos, bool lookingRight = true);

    void update(Level* level);
    void updateState(Level* level, const Vector2& playerPos);
    Vector2 getPosition() const { return position; }
    bool isLookingRight() const { return lookingRight; }
    State getState() const { return currentState; }

private:
    static constexpr float MOVEMENT_SPEED = 0.07f;
    static constexpr float CHASE_SPEED = 0.09f;
    static constexpr float DETECTION_RANGE = 5.0f;
    static constexpr float CHASE_RANGE = 7.0f;

    void patrol(Level* level);
    void chase(Level* level, const Vector2& playerPos);
    void returnToPatrol(Level* level);
    float distanceToPlayer(const Vector2& playerPos) const;
    bool canSeePlayer(Level* level, const Vector2& playerPos) const;

    Vector2 position;
    Vector2 patrolStart;
    bool lookingRight;
    State currentState;
    float patrolDistance;
    float initialPatrolDistance;
};

#endif // ENEMY_H