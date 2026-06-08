#pragma once
#include "raylib.h"

class map;
class battle_system;

class GameGraphics {
private:
    int cellSize;
    Texture2D playerTex;
    Texture2D enemyTex;
    Texture2D potionTex;

public:
    GameGraphics(int screenWidth, int screenHeight, int cSize) {
        cellSize = cSize;
        InitWindow(screenWidth, screenHeight, "Pokemon RPG");
        SetTargetFPS(60);


        playerTex = LoadTexture("project_pockemons/assets/player.png");
        if (playerTex.id == 0) playerTex = LoadTexture("assets/player.png"); // Резервный путь

        enemyTex = LoadTexture("project_pockemons/assets/enemy.png");
        if (enemyTex.id == 0) enemyTex = LoadTexture("assets/enemy.png");

        potionTex = LoadTexture("project_pockemons/assets/potion.png");
        if (potionTex.id == 0) potionTex = LoadTexture("assets/potion.png");
    }

    ~GameGraphics() {
        UnloadTexture(playerTex);
        UnloadTexture(enemyTex);
        UnloadTexture(potionTex);
        CloseWindow();
    }

    int GetCellSize() const { return cellSize; }
    Texture2D GetPlayerTex() const { return playerTex; }
    Texture2D GetEnemyTex() const { return enemyTex; }
    Texture2D GetPotionTex() const { return potionTex; }

    void Render(map& game_map, int player_x, int player_y);
    void RenderMenu();
    void RenderBattle(battle_system& battle);
    void RenderGameOver(bool victory);
};