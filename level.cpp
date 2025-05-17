#include "level.h"
#include "player.h"
#include "enemy.h"
#include <cmath>

Level::LevelData Level::LEVELS[LEVEL_COUNT] = {
    {12, 72, LEVEL_1_DATA},
    {12, 78, LEVEL_2_DATA},
    {12, 86, LEVEL_3_DATA}
};

Level::Level() : rows(0), columns(0), data(nullptr) {}

Level::~Level() {
    unload();
}

void Level::load(int index) {
    unload();

    rows = LEVELS[index].rows;
    columns = LEVELS[index].columns;
    data = new char[rows * columns];

    for (size_t row = 0; row < rows; row++) {
        for (size_t column = 0; column < columns; column++) {
            data[row * columns + column] = LEVELS[index].data[row * columns + column];
        }
    }
}

void Level::unload() {
    if (data) {
        delete[] data;
        data = nullptr;
    }
    rows = 0;
    columns = 0;
}

bool Level::isInside(int row, int column) const {
    if (row < 0 || row >= static_cast<int>(rows)) return false;
    if (column < 0 || column >= static_cast<int>(columns)) return false;
    return true;
}

bool Level::isColliding(Vector2 pos, char lookFor) const {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (int row = static_cast<int>(floor(pos.y)) - 1; row <= static_cast<int>(floor(pos.y)) + 1; ++row) {
        for (int column = static_cast<int>(floor(pos.x)) - 1; column <= static_cast<int>(floor(pos.x)) + 1; ++column) {
            if (!isInside(row, column)) continue;
            if (getCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float)column, (float)row, 1.0f, 1.0f};
                if (CheckCollisionRecs(entityHitbox, blockHitbox)) {
                    return true;
                }
            }
        }
    }
    return false;
}

char& Level::getCollider(Vector2 pos, char lookFor) {
    Rectangle playerHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (int row = static_cast<int>(floor(pos.y)) - 1; row <= static_cast<int>(floor(pos.y)) + 1; ++row) {
        for (int column = static_cast<int>(floor(pos.x)) - 1; column <= static_cast<int>(floor(pos.x)) + 1; ++column) {
            if (!isInside(row, column)) continue;
            if (getCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float)column, (float)row, 1.0f, 1.0f};
                if (CheckCollisionRecs(playerHitbox, blockHitbox)) {
                    return getCell(row, column);
                }
            }
        }
    }
    return getCell(static_cast<size_t>(floor(pos.y)), static_cast<size_t>(floor(pos.x)));
}

char& Level::getCell(size_t row, size_t column) {
    return data[row * columns + column];
}

const char& Level::getCell(size_t row, size_t column) const {
    return data[row * columns + column];
}

void Level::setCell(size_t row, size_t column, char chr) {
    data[row * columns + column] = chr;
}