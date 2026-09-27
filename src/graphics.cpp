#include "graphics.h"

#include <cmath>
#include <string>

#include "battle_system.h"
#include "world_map.h"

using namespace std;

GameGraphics::GameGraphics(int screenWidth, int screenHeight, int cSize) : cellSize(cSize) {
    InitWindow(screenWidth, screenHeight, "Pokemon RPG");
    SetTargetFPS(60);

    playerTex = LoadTexture("assets/player.png");
    enemyTex = LoadTexture("assets/enemy.png");
    potionTex = LoadTexture("assets/potion.png");
}

GameGraphics::~GameGraphics() {
    UnloadTexture(playerTex);
    UnloadTexture(enemyTex);
    UnloadTexture(potionTex);
    CloseWindow();
}

void GameGraphics::Render(const world_map& game_map, int player_x, int player_y) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    int w = game_map.get_width();
    int h = game_map.get_height();

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            cell_type type = game_map.get_cell_type(x, y);
            Rectangle rect = {(float)x * cellSize, (float)y * cellSize, (float)cellSize, (float)cellSize};

            if (type == cell_type::wall) {
                DrawRectangleRec(rect, DARKGRAY);
            } else if (type == cell_type::potion) {
                if (potionTex.id > 0) DrawTexturePro(potionTex, {0,0,(float)potionTex.width,(float)potionTex.height}, rect, {0,0}, 0.0f, WHITE);
                else DrawRectangleRec(rect, GREEN);
            } else if (type == cell_type::enemy) {
                if (enemyTex.id > 0) DrawTexturePro(enemyTex, {0,0,(float)enemyTex.width,(float)enemyTex.height}, rect, {0,0}, 0.0f, WHITE);
                else DrawRectangleRec(rect, RED);
            }
            DrawRectangleLinesEx(rect, 1, LIGHTGRAY);
        }
    }

    Rectangle playerRect = {(float)player_x * cellSize, (float)player_y * cellSize, (float)cellSize, (float)cellSize};
    if (playerTex.id > 0) DrawTexturePro(playerTex, {0,0,(float)playerTex.width,(float)playerTex.height}, playerRect, {0,0}, 0.0f, WHITE);
    else DrawRectangleRec(playerRect, BLUE);

    int textY = h * cellSize + 15;
    DrawText("Controls: W A S D | Save & Exit: K", 10, textY, 20, BLACK);
    DrawText("You: Blue | Enemies: Red | Potions: Green", 10, textY + 25, 18, DARKGRAY);

    EndDrawing();
}

void GameGraphics::RenderMenu() {
    BeginDrawing();
    ClearBackground(DARKBLUE);

    DrawText("POKEMON RPG", 240, 200, 50, GOLD);
    DrawText("1. Start New Game (Press 1)", 160, 360, 22, WHITE);
    DrawText("2. Continue Game (Press 2)", 160, 410, 22, WHITE);
    DrawText("Exit: Close window or press ESC", 160, 500, 18, LIGHTGRAY);

    EndDrawing();
}

void GameGraphics::RenderBattle(const battle_system& battle) {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawRectangle(40, 40, 720, 720, CLITERAL(Color){ 235, 245, 235, 255 });
    DrawRectangleLines(40, 40, 720, 720, GRAY);

    float bobbingPlayer = sin(GetTime() * 4.0f) * 8.0f;
    float bobbingEnemy = cos(GetTime() * 4.0f) * 8.0f;

    Rectangle pPos = {120, 400 + bobbingPlayer, 160, 160};
    if (playerTex.id > 0) DrawTexturePro(playerTex, {0,0,(float)playerTex.width,(float)playerTex.height}, pPos, {0,0}, 0.0f, WHITE);
    else DrawRectangleRec(pPos, BLUE);

    Rectangle ePos = {520, 120 + bobbingEnemy, 160, 160};
    if (enemyTex.id > 0) DrawTexturePro(enemyTex, {0,0,(float)enemyTex.width,(float)enemyTex.height}, ePos, {0,0}, 0.0f, WHITE);
    else DrawRectangleRec(ePos, RED);

    if (!battle.enemies.empty()) {
        pokemon* ev = battle.enemies[0];
        DrawText(ev->get_name().c_str(), 480, 295, 22, BLACK);
        DrawText(TextFormat("HP: %d/%d", ev->get_hp(), ev->get_max_hp()), 480, 325, 18, DARKGRAY);
        DrawRectangle(480, 350, 200, 12, RED);
        float hpPerc = (float)ev->get_hp() / ev->get_max_hp();
        if (hpPerc < 0) hpPerc = 0;
        DrawRectangle(480, 350, (int)(200 * hpPerc), 12, GREEN);
    }

    DrawText("YOUR TEAM:", 80, 80, 22, DARKBLUE);
    for (int i = 0; i < 3; i++) {
        if (battle.p_ref.team[i] != nullptr) {
            string info = to_string(i + 1) + ". " + battle.p_ref.team[i]->get_name() +
                          " [HP: " + to_string(battle.p_ref.team[i]->get_hp()) + "/" + to_string(battle.p_ref.team[i]->get_max_hp()) + "]";
            if (battle.p_ref.team[i]->is_fail()) info += " (FAINTED)";

            Color col = (i == battle.active_my_idx) ? MAROON : BLACK;
            DrawText(info.c_str(), 80, 115 + i * 30, 18, col);
        }
    }

    DrawRectangle(60, 590, 680, 130, RAYWHITE);
    DrawRectangleLines(60, 590, 680, 130, BLACK);
    DrawText(battle.log_msg1.c_str(), 80, 610, 20, BLACK);
    DrawText(battle.log_msg2.c_str(), 80, 650, 18, DARKBLUE);

    if (battle.phase != battle_phase::select_pokemon) {
        DrawText("[ Press SPACE to continue ]", 250, 695, 15, GRAY);
    }

    EndDrawing();
}

void GameGraphics::RenderGameOver(bool victory) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (victory) {
        DrawText("VICTORY!", 320, 300, 45, GOLD);
        DrawText("You cleared the map of wild enemies!", 200, 380, 22, WHITE);
    } else {
        DrawText("GAME OVER", 240, 300, 45, RED);
        DrawText("Your team was completely defeated.", 160, 380, 22, WHITE);
    }
    DrawText("Press any key to exit", 230, 520, 18, LIGHTGRAY);

    EndDrawing();
}
