#include "game.h"

#include <cstdlib>
#include <fstream>

#include "battle_system.h"
#include "raylib.h"

using namespace std;

game::game() : state(game_state::menu), ash(0, 0), game_map() {
    pokemon_names = load_pokemon_names();
}

void game::MainMenu(GameGraphics& gfx) {
    state = game_state::menu;
    Run(gfx);
}

void game::Run(GameGraphics& gfx) {
    battle_system* current_battle = nullptr;

    while (!WindowShouldClose()) {
        if (state == game_state::menu) {
            gfx.RenderMenu();

            if (IsKeyPressed(KEY_ONE)) {
                InitializeNewPlayerTeam();
                game_map.generate_random_map();
                state = game_state::exploration;
            } else if (IsKeyPressed(KEY_TWO)) {
                if (load_game()) {
                    state = game_state::exploration;
                } else {
                    InitializeNewPlayerTeam();
                    game_map.generate_random_map();
                    state = game_state::exploration;
                }
            }
        } else if (state == game_state::exploration) {
            int dx = 0, dy = 0;
            bool moved = false;

            if (IsKeyPressed(KEY_W)) { dy = -1; moved = true; }
            else if (IsKeyPressed(KEY_S)) { dy = 1; moved = true; }
            else if (IsKeyPressed(KEY_A)) { dx = -1; moved = true; }
            else if (IsKeyPressed(KEY_D)) { dx = 1; moved = true; }
            else if (IsKeyPressed(KEY_K)) {
                save_game();
                break;
            }

            if (moved) {
                int nx = ash.get_x() + dx;
                int ny = ash.get_y() + dy;

                if (game_map.can_move(nx, ny)) {
                    ash.set_position(nx, ny);

                    cell_type cell = game_map.get_cell_type(nx, ny);
                    if (cell == cell_type::potion) {
                        ash.heal_team();
                        game_map.set_empty_cell(nx, ny);
                    } else if (cell == cell_type::enemy) {
                        string e_name = pokemon_names[rand() % pokemon_names.size()];
                        string types[] = { "fire", "water", "air", "earth" };
                        pokemon* wild_p = new pokemon("Wild_" + e_name, types[rand() % 4]);

                        vector<pokemon*> wildEnemies;
                        wildEnemies.push_back(wild_p);

                        current_battle = new battle_system(ash);
                        current_battle->StartBattle(wildEnemies);
                        state = game_state::battle;
                    }

                    if (rand() % 100 < 15) {
                        game_map.spawn_random_potion();
                    }
                }
            }

            if (game_map.count_enemies() == 0) {
                state = game_state::gameover;
            }

            gfx.Render(game_map, ash.get_x(), ash.get_y());
        } else if (state == game_state::battle) {
            if (current_battle) {
                current_battle->Update();
                gfx.RenderBattle(*current_battle);

                if (current_battle->phase == battle_phase::battle_end && IsKeyPressed(KEY_SPACE)) {
                    if (current_battle->state == battle_state::victory) {
                        game_map.set_empty_cell(ash.get_x(), ash.get_y());
                        // 50% что выпадет зелье
                        if (rand() % 100 < 50) {
                            game_map.set_potion_cell(ash.get_x(), ash.get_y());
                        }
                        state = game_state::exploration;
                    } else {
                        state = game_state::gameover;
                    }
                    delete current_battle;
                    current_battle = nullptr;
                }
            }
        } else if (state == game_state::gameover) {
            bool victory = (game_map.count_enemies() == 0);
            gfx.RenderGameOver(victory);

            if (GetKeyPressed() > 0) {
                break;
            }
        }
    }

    delete current_battle;
}

void game::InitializeNewPlayerTeam() {
    string types[] = { "fire", "water", "air", "earth" };
    for (int i = 0; i < 3; i++) {
        string r_name = pokemon_names[rand() % pokemon_names.size()];
        string r_type = types[rand() % 4];
        ash.add_pokemon_to_team(new pokemon(r_name, r_type));
    }
}

void game::save_game() {
    ofstream out("savegame.txt");
    if (out.is_open()) {
        ash.save(out);
        game_map.save(out);
    }
}

bool game::load_game() {
    ifstream in("savegame.txt");
    if (!in.is_open()) return false;

    ash.load(in);
    game_map.load(in);

    return static_cast<bool>(in);
}
