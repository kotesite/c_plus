#pragma once

#include "raylib.h"

class world_map;
class battle_system;

class GameGraphics {
private:
    int cellSize;
    Texture2D playerTex;
    Texture2D enemyTex;
    Texture2D potionTex;

public:
    GameGraphics(int screenWidth, int screenHeight, int cSize);
    ~GameGraphics();

    int GetCellSize() const { return cellSize; }

    void Render(const world_map& game_map, int player_x, int player_y);
    void RenderMenu();
    void RenderBattle(const battle_system& battle);
    void RenderGameOver(bool victory);
};
