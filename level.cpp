#include "level.h"
#include "player.h"
#include "enemy.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

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
    try {
        loadFromRLL("data/levels.rll", index);
    } catch (const LevelLoadException& e) {
        // Fallback to hardcoded levels if RLL loading fails
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
}

void Level::loadFromRLL(const std::string& filename, int levelIndex) {
    std::vector<std::string> levels = parseRLLFile(filename);

    if (levelIndex < 0 || levelIndex >= static_cast<int>(levels.size())) {
        throw LevelLoadException("Invalid level index");
    }

    std::string decodedLevel = decodeRLEString(levels[levelIndex]);
    createLevelFromRLE(decodedLevel);
}

std::vector<std::string> Level::parseRLLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw LevelLoadException("Could not open level file: " + filename);
    }

    std::vector<std::string> levels;
    std::string line;
    std::string currentLevel;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') {
            if (!currentLevel.empty()) {
                levels.push_back(currentLevel);
                currentLevel.clear();
            }
            continue;
        }
        currentLevel += line;
    }

    if (!currentLevel.empty()) {
        levels.push_back(currentLevel);
    }

    if (levels.empty()) {
        throw LevelLoadException("No levels found in file");
    }

    return levels;
}

std::string Level::decodeRLEString(const std::string& rleString) {
    std::string result;
    std::string number;

    for (size_t i = 0; i < rleString.length(); i++) {
        char c = rleString[i];

        if (isdigit(c)) {
            number += c;
        } else {
            int count = number.empty() ? 1 : std::stoi(number);

            if (c == '|') {
                result += '\n';
            } else {
                result.append(count, c);
            }
            number.clear();
        }
    }

    return result;
}

void Level::createLevelFromRLE(const std::string& decodedLevel) {
    unload();

    std::istringstream stream(decodedLevel);
    std::vector<std::string> levelRows;
    std::string row;

    size_t maxWidth = 0;
    while (std::getline(stream, row)) {
        levelRows.push_back(row);
        maxWidth = std::max(maxWidth, row.length());
    }

    rows = levelRows.size();
    columns = maxWidth;

    if (rows == 0 || columns == 0) {
        throw LevelLoadException("Invalid level dimensions");
    }

    data = new char[rows * columns];

    // Initialize with air
    for (size_t i = 0; i < rows * columns; i++) {
        data[i] = AIR;
    }

    // Copy level data
    for (size_t row = 0; row < rows; row++) {
        const std::string& currentRow = levelRows[row];
        for (size_t col = 0; col < currentRow.length(); col++) {
            data[row * columns + col] = currentRow[col];
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