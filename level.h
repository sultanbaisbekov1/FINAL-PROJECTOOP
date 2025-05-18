#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "globals.h"
#include <vector>
#include <cstddef>
#include <string>
#include <stdexcept>

class Player;
class Enemy;

class Level {
public:
    Level();
    ~Level();

    void load(int index);
    void loadFromRLL(const std::string& filename, int levelIndex);
    void unload();

    bool isInside(int row, int column) const;
    bool isColliding(Vector2 pos, char lookFor) const;
    char& getCollider(Vector2 pos, char lookFor);
    char& getCell(size_t row, size_t column);
    const char& getCell(size_t row, size_t column) const;
    void setCell(size_t row, size_t column, char chr);

    size_t getRows() const { return rows; }
    size_t getColumns() const { return columns; }
    char* getData() const { return data; }
    char getWallChar() const { return WALL; }
    char getWallDarkChar() const { return WALL_DARK; }
    char getSpikeChar() const { return SPIKE; }
    char getCoinChar() const { return COIN; }
    char getExitChar() const { return EXIT; }
    char getPlayerChar() const { return PLAYER; }
    char getEnemyChar() const { return ENEMY; }
    char getAirChar() const { return AIR; }

private:
    struct LevelData {
        size_t rows;
        size_t columns;
        char* data;
    };
    static LevelData LEVELS[LEVEL_COUNT];

    std::vector<std::string> parseRLLFile(const std::string& filename);
    std::string decodeRLEString(const std::string& rleString);
    void parseLevelDimensions(const std::string& decodedLevel);
    void createLevelFromRLE(const std::string& decodedLevel);

    size_t rows;
    size_t columns;
    char* data;
};

class LevelLoadException : public std::runtime_error {
public:
    explicit LevelLoadException(const std::string& message) : std::runtime_error(message) {}
};

#endif // LEVEL_H