#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"
#include <vector>
#include <string>
#include <cstddef>

class Level;
class Player;
class Enemy;

class Graphics {
public:
    Graphics(Player* player);
    ~Graphics();

    void drawMenu();
    void drawGame(Level* level, const std::vector<Enemy*>& enemies, size_t gameFrame);
    void drawDeathScreen(Level* level, const std::vector<Enemy*>& enemies, size_t gameFrame);
    void drawGameOverMenu();
    void drawPauseMenu();
    void drawVictoryMenu(size_t gameFrame);
    void initializeVictoryBalls();

private:
    struct Text {
        std::string str;
        Vector2 position;
        float size;
        Color color;
        float spacing;
        Font* font;
    };

    struct Sprite {
        size_t frameCount;
        size_t framesToSkip;
        size_t framesSkipped;
        size_t frameIndex;
        bool loop;
        size_t prevGameFrame;
        Texture2D* frames;
    };

    struct VictoryBall {
        float x, y;
        float dx, dy;
        float radius;
    };

    void loadAssets();
    void unloadAssets();
    void drawText(const Text& text);
    void drawSprite(Sprite& sprite, Vector2 pos, float size, size_t gameFrame);
    void drawImage(Texture2D image, Vector2 pos, float size);
    void deriveMetricsFromLevel(Level* level);
    void drawParallaxBackground(size_t gameFrame);

    // Assets
    Font menuFont;
    Texture2D wallImage;
    Texture2D wallDarkImage;
    Texture2D spikeImage;
    Texture2D exitImage;
    Sprite coinSprite;
    Texture2D heartImage;
    Texture2D playerStandForwardImage;
    Texture2D playerStandBackwardsImage;
    Texture2D playerJumpForwardImage;
    Texture2D playerJumpBackwardsImage;
    Texture2D playerDeadImage;
    Sprite playerWalkForwardSprite;
    Sprite playerWalkBackwardsSprite;
    Sprite enemyWalkSprite;
    Texture2D backgroundImage;
    Texture2D middlegroundImage;
    Texture2D foregroundImage;

    Text gameTitle;
    Text gameSubtitle;
    Text gamePaused;
    Text deathTitle;
    Text deathSubtitle;
    Text gameOverTitle;
    Text gameOverSubtitle;
    Text victoryTitle;
    Text victorySubtitle;

    static const size_t VICTORY_BALL_COUNT = 2000;
    static constexpr float VICTORY_BALL_MAX_SPEED = 2.0f;
    static constexpr float VICTORY_BALL_MIN_RADIUS = 2.0f;
    static constexpr float VICTORY_BALL_MAX_RADIUS = 3.0f;
    static const Color VICTORY_BALL_COLOR;
    static const unsigned char VICTORY_BALL_TRAIL_TRANSPARENCY = 10;
    VictoryBall victoryBalls[VICTORY_BALL_COUNT];

    Player* player;

    Vector2 screenSize;
    float screenScale;
    float cellSize;
    float horizontalShift;
    Vector2 backgroundSize;
    float backgroundYOffset;
    static constexpr float SCREEN_SCALE_DIVISOR = 700.0f;
    static constexpr float PARALLAX_PLAYER_SCROLLING_SPEED = 0.003f;
    static constexpr float PARALLAX_IDLE_SCROLLING_SPEED = 0.00005f;
    static constexpr float PARALLAX_LAYERED_SPEED_DIFFERENCE = 3.0f;
};

#endif // GRAPHICS_H