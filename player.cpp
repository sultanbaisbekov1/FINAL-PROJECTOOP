#include "player.h"
#include "level.h"
#include "enemy.h"
#include <cmath>

Player::Player() :
    yVelocity(0),
    onGround(false),
    lookingForward(true),
    moving(false),
    dead(false),
    lives(MAX_PLAYER_LIVES),
    timer(MAX_LEVEL_TIME),
    timeToCoinCounter(0) {
    for (int i = 0; i < LEVEL_COUNT; i++) {
        levelScores[i] = 0;
    }
}

void Player::resetStats() {
    lives = MAX_PLAYER_LIVES;
    dead = false;
    timer = MAX_LEVEL_TIME;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        levelScores[i] = 0;
    }
}

void Player::incrementScore() {
    levelScores[level_index] += 10;
}

int Player::getTotalScore() const {
    int total = 0;
    for (int i = 0; i < LEVEL_COUNT; i++) {
        total += levelScores[i];
    }
    return total;
}

void Player::spawn(Level* level) {
    bool found = false;
    for (size_t row = 0; row < level->getRows(); ++row) {
        for (size_t column = 0; column < level->getColumns(); ++column) {
            if (level->getCell(row, column) == PLAYER) {
                position = {static_cast<float>(column), static_cast<float>(row)};
                yVelocity = 0;
                onGround = false;
                dead = false;
                level->setCell(row, column, AIR);
                found = true;
                return;
            }
        }
    }
    if (!found) {
        position = {1.0f, static_cast<float>(level->getRows() - 2)};
        yVelocity = 0;
        onGround = false;
        dead = false;
    }
}

void Player::kill() {
    if (!dead) {
        dead = true;
        yVelocity = -JUMP_STRENGTH * 0.5f;
        lives--;
    }
}

void Player::moveHorizontally(float delta, Level* level) {
    if (dead) return;
    moving = delta != 0;
    if (moving) {
        lookingForward = delta > 0;
    }

    float newX = position.x + delta;
    if (!level->isColliding({newX, position.y}, level->getWallChar())) {
        position.x = newX;
    }
}

void Player::jump() {
    if (onGround && !dead) {
        yVelocity = -JUMP_STRENGTH;
        onGround = false;
    }
}

void Player::update(Level* level, std::vector<Enemy*>& enemies, Sound coinSound, Sound exitSound,
                   Sound killEnemySound, Sound playerDeathSound, size_t gameFrame) {
    if (dead) return;

    if (timer > 0) {
        timer--;
    }

    if (level->isColliding(position, COIN)) {
        char& cell = level->getCollider(position, COIN);
        cell = AIR;
        incrementScore();
        if (IsAudioDeviceReady()) PlaySound(coinSound);
    }

    if (level->isColliding(position, EXIT)) {
        if (IsAudioDeviceReady()) PlaySound(exitSound);
    }

    for (auto it = enemies.begin(); it != enemies.end();) {
        Enemy* enemy = *it;
        Vector2 enemyPos = enemy->getPosition();

        Rectangle playerBox = { position.x - 0.3f, position.y - 0.3f, 0.6f, 0.6f };
        Rectangle enemyBox = { enemyPos.x - 0.3f, enemyPos.y - 0.3f, 0.6f, 0.6f };

        if (CheckCollisionRecs(playerBox, enemyBox)) {
            bool playerAboveEnemy = (position.y + 0.2f) < (enemyPos.y - 0.1f);

            if (playerAboveEnemy && yVelocity > 0) {
                yVelocity = -BOUNCE_OFF_ENEMY;
                if (IsAudioDeviceReady()) PlaySound(killEnemySound);
                delete enemy;
                it = enemies.erase(it);
            } else {
                kill();
                if (IsAudioDeviceReady()) PlaySound(playerDeathSound);
                break;
            }
        } else {
            ++it;
        }
    }

    if (level->isColliding(position, SPIKE)) {
        kill();
        if (IsAudioDeviceReady()) PlaySound(playerDeathSound);
    }

    updateGravity(level);
}

void Player::updateGravity(Level* level) {
    yVelocity += GRAVITY_FORCE;
    float newY = position.y + yVelocity;

    if (yVelocity > 0) {
        if (level->isColliding({position.x, newY}, level->getWallChar())) {
            position.y = floor(newY);
            yVelocity = 0;
            onGround = true;
        } else {
            position.y = newY;
            onGround = false;
        }
    } else {
        if (level->isColliding({position.x, newY}, level->getWallChar())) {
            position.y = ceil(newY);
            yVelocity = CEILING_BOUNCE_OFF;
        } else {
            position.y = newY;
        }
    }

    if (position.y > level->getRows()) {
        kill();
    }
}