#ifndef ENEMY_H
#define ENEMY_H

#include "globals.h"

void spawn_enemies() {
    // Create enemies, incrementing their amount every time a new one is created
    enemies.clear();

    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t column = 0; column < current_level.columns; ++column) {
            char cell = get_level_cell(row, column);

            if (cell == ENEMY) {
                // Instantiate and add an enemy to the level
                enemies.push_back({
                        {static_cast<float>(column), static_cast<float>(row)},
                        true
                });

                set_level_cell(row, column, AIR);
            }
        }
    }
}

void update_enemies() {
    for (auto &enemy : enemies) {
        // Find the enemy's next x
        float next_x = enemy.pos.x;
        next_x += (enemy.is_looking_right ? ENEMY_MOVEMENT_SPEED : -ENEMY_MOVEMENT_SPEED);

        // If its next position collides with a wall, turn around
        if (is_colliding({next_x, enemy.pos.y}, WALL)) {
            enemy.is_looking_right = !enemy.is_looking_right;
        }
        // Otherwise, keep moving
        else {
            enemy.pos.x = next_x;
        }
    }
}

// Custom is_colliding function for enemies
bool is_colliding_with_enemies(Vector2 pos) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto &enemy : enemies) {
        Rectangle enemy_hitbox = {(float) enemy.pos.x, (float) enemy.pos.y, 1.0f, 1.0f};
        if (CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            return true;
        }
    }
    return false;
}

void remove_colliding_enemy(Vector2 pos) {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto it = enemies.begin(); it != enemies.end(); it++) {
        Rectangle enemy_hitbox = {(float) it->pos.x, (float) it->pos.y, 1.0f, 1.0f};
        // Erase a colliding enemy
        if (CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            enemies.erase(it);

            // Call the function again to remove any remaining colliding enemies
            // Cannot continue as calling erase on a vector invalidates current iterators
            remove_colliding_enemy(pos);
            return;
        }
    }
}

#endif //ENEMY_H
