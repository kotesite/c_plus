#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <fstream>
#include <cmath>
#include "raylib.h"
#include "graphics.h"

using namespace std;

class pokemon;
class ability;

vector<string> load_pokemon_names() {
    vector<string> names;
    ifstream file("pokemon_names.txt");
    if (!file.is_open()) {
        ofstream outfile("pokemon_names.txt");
        outfile << "Pikachu\nCharizard\nBulbasaur\nSquirtle\nZubat\nGeodude\nEevee\nSnorlax\nMewtwo\nAbra\n";
        outfile.close();
        names = { "Pikachu", "Charizard", "Bulbasaur", "Squirtle", "Zubat", "Geodude", "Eevee", "Snorlax", "Mewtwo", "Abra" };
    } else {
        string name;
        while (file >> name) {
            names.push_back(name);
        }
        file.close();
    }
    if (names.empty()) names = { "MissingNo" };
    return names;
}

class ability {
private:
    string name_ability;
    int base_damage;

public:
    ability(string n, int dmg) : name_ability(n), base_damage(dmg) {}
    string get_name() const { return name_ability; }
    int get_damage() const { return base_damage; }
    bool Damage(pokemon& target);
};

class pokemon {
private:
    string name;
    int level;
    int level_evolutions;
    string type;
    int hp;
    int max_hp;
    int exp;
    int exp_next_level;
    int attack;

    vector<ability*> abilities;

    friend class player;
    friend class battle_system;
    friend bool ability::Damage(pokemon& target);

    static float get_type_koef(const string& t) {
        if (t == "fire") return 2.8f;
        if (t == "water") return 2.6f;
        if (t == "air") return 2.5f;
        if (t == "earth") return 2.7f;
        return 1.0f;
    }

public:
    pokemon(string n, string type_pokemon)
        : name(n), level(1), level_evolutions(0), type(type_pokemon),
          hp(static_cast<int>(100 * get_type_koef(type_pokemon))),
          max_hp(static_cast<int>(100 * get_type_koef(type_pokemon))),
          exp(0), exp_next_level(100), attack(10) {
        add_ability(new ability("Basic Strike", 20));
    }

    ~pokemon() {
        for (ability* a : abilities) delete a;
    }

    void add_ability(ability* a) { abilities.push_back(a); }
    string get_name() const { return name; }

    int get_hp() const { return hp; }
    int get_max_hp() const { return max_hp; }

    const vector<ability*>& get_abilities() const { return abilities; }

    void restore_hp() {
        hp = max_hp;
    }

    bool is_fail() const { return hp <= 0; }

    void evolution(float koef_type) {
        if (level >= 5 && level_evolutions == 0) {
            name += " Small";
            max_hp += static_cast<int>(15 * koef_type);
            attack += static_cast<int>(15 * koef_type);
            hp = max_hp;
            level_evolutions++;
        } else if (level >= 10 && level_evolutions == 1) {
            name += " Middin";
            max_hp += static_cast<int>(20 * koef_type);
            attack += static_cast<int>(20 * koef_type);
            hp = max_hp;
            level_evolutions++;
        } else if (level >= 15 && level_evolutions == 2) {
            name += " Bigin";
            max_hp += static_cast<int>(25 * koef_type);
            attack += static_cast<int>(25 * koef_type);
            hp = max_hp;
            level_evolutions++;
        }
    }

    void next_level(int income_exp, float koef_type) {
        exp += income_exp;
        while (exp >= exp_next_level) {
            exp -= exp_next_level;
            level++;
            max_hp = max_hp + static_cast<int>(level * 2 * koef_type);
            attack = attack + static_cast<int>(koef_type * pow(level, 2));
            hp = max_hp;
            exp_next_level = static_cast<int>(exp_next_level * 1.5);
        }
        evolution(koef_type);
    }

    void save(ofstream& out) {
        out << name << " " << level << " " << level_evolutions << " " << type << " "
            << hp << " " << max_hp << " " << exp << " " << exp_next_level << " " << attack << "\n";
    }

    void load(ifstream& in) {
        in >> name >> level >> level_evolutions >> type >> hp >> max_hp >> exp >> exp_next_level >> attack;
    }
};

bool ability::Damage(pokemon& target) {
    int final_damage = this->base_damage;
    target.hp -= final_damage;
    if (target.hp < 0) target.hp = 0;
    return target.is_fail();
}

class player {
private:
    int x, y;
    pokemon* team[3];
    int pokemon_count;

    friend class battle_system;
    friend class GameGraphics;

public:
    player(int start_x, int start_y) : x(start_x), y(start_y), pokemon_count(0) {
        for (int i = 0; i < 3; i++) team[i] = nullptr;
    }

    ~player() {
        for (int i = 0; i < pokemon_count; i++) delete team[i];
    }

    void add_pokemon_to_team(pokemon* p) {
        if (pokemon_count < 3) team[pokemon_count++] = p;
    }

    pokemon* get_active_pokemon() {
        for (int i = 0; i < pokemon_count; i++) {
            if (team[i] != nullptr && !team[i]->is_fail()) return team[i];
        }
        return nullptr;
    }

    void heal_team() {
        for (int i = 0; i < pokemon_count; i++) {
            if (team[i] != nullptr) team[i]->restore_hp();
        }
    }

    void set_position(int nx, int ny) { x = nx; y = ny; }
    int get_x() const { return x; }
    int get_y() const { return y; }

    void save(ofstream& out) {
        out << x << " " << y << " " << pokemon_count << "\n";
        for (int i = 0; i < pokemon_count; i++) {
            team[i]->save(out);
        }
    }

    void load(ifstream& in) {
        for (int i = 0; i < pokemon_count; i++) {
            if (team[i] != nullptr) {
                delete team[i];
                team[i] = nullptr;
            }
        }
        in >> x >> y >> pokemon_count;
        for (int i = 0; i < pokemon_count; i++) {
            pokemon* p = new pokemon("Temp", "water");
            p->load(in);
            team[i] = p;
        }
    }
};

enum class cell_type { empty, wall, enemy, potion };

class map {
private:
    int width;
    int height;
    vector<vector<cell_type>> grid;
    int enemy_count;

public:
    map() : width(10), height(10), enemy_count(0) {}

    int get_width() const { return width; }
    int get_height() const { return height; }

    bool can_move(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        if (grid[x][y] == cell_type::wall) return false;
        return true;
    }

    int count_enemies() const { return enemy_count; }
    cell_type get_cell_type(int x, int y) const { return grid[x][y]; }

    void set_empty_cell(int x, int y) {
        if (grid[x][y] == cell_type::enemy) enemy_count--;
        grid[x][y] = cell_type::empty;
    }

    void set_potion_cell(int x, int y) {
        grid[x][y] = cell_type::potion;
    }

    void spawn_random_potion() {
        vector<pair<int, int>> empty_cells;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (grid[i][j] == cell_type::empty) {
                    empty_cells.push_back({i, j});
                }
            }
        }
        if (!empty_cells.empty()) {
            int idx = rand() % empty_cells.size();
            grid[empty_cells[idx].first][empty_cells[idx].second] = cell_type::potion;
        }
    }

    void generate_random_map() {
        width = rand() % 6 + 10;
        height = rand() % 6 + 10;
        int target_enemies = rand() % 7 + 3;

        bool is_map_valid = false;
        while (!is_map_valid) {
            grid.assign(width, vector<cell_type>(height, cell_type::empty));
            int total_walls = (width * height) / 4;
            int walls_placed = 0;
            while (walls_placed < total_walls) {
                int rx = rand() % width;
                int ry = rand() % height;
                if ((rx != 0 || ry != 0) && grid[rx][ry] == cell_type::empty) {
                    grid[rx][ry] = cell_type::wall;
                    walls_placed++;
                }
            }

            vector<pair<int, int>> reachable_cells;
            vector<vector<bool>> visited(width, vector<bool>(height, false));
            queue<pair<int, int>> q;

            q.push(make_pair(0, 0));
            visited[0][0] = true;

            int dx[] = { -1, 1, 0, 0 };
            int dy[] = { 0, 0, -1, 1 };

            while (!q.empty()) {
                pair<int, int> curr = q.front();
                q.pop();

                if (curr.first != 0 || curr.second != 0) {
                    reachable_cells.push_back(curr);
                }

                for (int i = 0; i < 4; i++) {
                    int nx = curr.first + dx[i];
                    int ny = curr.second + dy[i];

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        if (!visited[nx][ny] && grid[nx][ny] != cell_type::wall) {
                            visited[nx][ny] = true;
                            q.push(make_pair(nx, ny));
                        }
                    }
                }
            }

            if (reachable_cells.size() < static_cast<size_t>(target_enemies + 3)) {
                continue;
            }

            is_map_valid = true;

            for (int i = 0; i < 3; i++) {
                int index = rand() % reachable_cells.size();
                pair<int, int> pos = reachable_cells[index];
                grid[pos.first][pos.second] = cell_type::potion;
                reachable_cells.erase(reachable_cells.begin() + index);
            }

            for (int i = 0; i < target_enemies; i++) {
                int index = rand() % reachable_cells.size();
                pair<int, int> pos = reachable_cells[index];
                grid[pos.first][pos.second] = cell_type::enemy;
                reachable_cells.erase(reachable_cells.begin() + index);
            }

            enemy_count = target_enemies;
        }
    }

    void save(ofstream& out) {
        out << width << " " << height << " " << enemy_count << "\n";
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                out << static_cast<int>(grid[i][j]) << " ";
            }
            out << "\n";
        }
    }

    void load(ifstream& in) {
        in >> width >> height >> enemy_count;
        grid.assign(width, vector<cell_type>(height, cell_type::empty));
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int val;
                in >> val;
                grid[i][j] = static_cast<cell_type>(val);
            }
        }
    }
};

enum class battle_state { fighting, victory, defeat };
enum class battle_phase { select_pokemon, player_anim, enemy_anim, battle_end };

class battle_system {
public:
    battle_state state;
    battle_phase phase;
    vector<pokemon*> enemies;
    player& p_ref;
    string log_msg1;
    string log_msg2;
    int active_my_idx;

    battle_system(player& p) : p_ref(p), state(battle_state::fighting), phase(battle_phase::select_pokemon), active_my_idx(-1) {
        log_msg1 = "Battle begins!";
        log_msg2 = "Select a Pokemon to attack with [1, 2, 3]";
    }

    ~battle_system() {
        for (pokemon* e : enemies) delete e;
    }

    void StartBattle(std::vector<pokemon*> encountered_enemies) {
        enemies = encountered_enemies;
        log_msg1 = "Wild " + enemies[0]->get_name() + " appears!";
        log_msg2 = "Select your Pokemon: keys [1], [2], or [3]";
    }

    void Update() {
        if (state != battle_state::fighting) return;
        pokemon* enemy_active = enemies[0];

        if (phase == battle_phase::select_pokemon) {
            int choice = -1;
            if (IsKeyPressed(KEY_ONE)) choice = 0;
            if (IsKeyPressed(KEY_TWO)) choice = 1;
            if (IsKeyPressed(KEY_THREE)) choice = 2;

            if (choice >= 0 && choice < 3 && p_ref.team[choice] != nullptr && !p_ref.team[choice]->is_fail()) {
                active_my_idx = choice;
                pokemon* my_active = p_ref.team[active_my_idx];

                log_msg1 = my_active->get_name() + " uses " + my_active->get_abilities()[0]->get_name() + "!";
                int dmg_to_enemy = my_active->get_abilities()[0]->get_damage();
                bool enemy_dead = my_active->get_abilities()[0]->Damage(*enemy_active);
                log_msg2 = enemy_active->get_name() + " took " + to_string(dmg_to_enemy) + " damage.";

                if (enemy_dead) {
                    state = battle_state::victory;
                    phase = battle_phase::battle_end;
                    my_active->next_level(120, 1.4f);
                    log_msg2 += " Enemy fainted! Press SPACE.";
                } else {
                    phase = battle_phase::player_anim;
                }
            }
        }
        else if (phase == battle_phase::player_anim) {
            if (IsKeyPressed(KEY_SPACE)) {
                pokemon* my_active = p_ref.team[active_my_idx];
                log_msg1 = enemy_active->get_name() + " counter-attacks!";
                int dmg_to_me = enemy_active->get_abilities()[0]->get_damage();
                enemy_active->get_abilities()[0]->Damage(*my_active);
                log_msg2 = my_active->get_name() + " took " + to_string(dmg_to_me) + " damage.";

                if (my_active->is_fail()) {
                    log_msg2 += " Fainted!";
                }
                phase = battle_phase::enemy_anim;
            }
        }
        else if (phase == battle_phase::enemy_anim) {
            if (IsKeyPressed(KEY_SPACE)) {
                int alive_count = 0;
                for (int i = 0; i < 3; i++) {
                    if (p_ref.team[i] != nullptr && !p_ref.team[i]->is_fail()) alive_count++;
                }

                if (alive_count == 0) {
                    state = battle_state::defeat;
                    phase = battle_phase::battle_end;
                    log_msg1 = "All your Pokemon fainted!";
                    log_msg2 = "Battle lost. Press SPACE.";
                } else {
                    phase = battle_phase::select_pokemon;
                    log_msg1 = "Choose Pokemon for next attack:";
                    log_msg2 = "Keys [1], [2], or [3]";
                }
            }
        }
    }
};



void GameGraphics::Render(map& game_map, int player_x, int player_y) {
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

void GameGraphics::RenderBattle(battle_system& battle) {
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



enum class game_state { menu, exploration, battle, gameover };

class game {
private:
    game_state state;
    player ash;
    map game_map;
    vector<string> pokemon_names;

public:
    game() : state(game_state::menu), ash(0, 0), game_map() {
        pokemon_names = load_pokemon_names();
    }

    void MainMenu(GameGraphics& gfx) {
        state = game_state::menu;
        Run(gfx);
    }

    void Run(GameGraphics& gfx) {
        battle_system* current_battle = nullptr;

        while (!WindowShouldClose()) {

            if (state == game_state::menu) {
                gfx.RenderMenu();

                if (IsKeyPressed(KEY_ONE)) {
                    InitializeNewPlayerTeam();
                    game_map.generate_random_map();
                    state = game_state::exploration;
                } else if (IsKeyPressed(KEY_TWO)) {
                    if (load_game()) state = game_state::exploration;
                    else {
                        InitializeNewPlayerTeam();
                        game_map.generate_random_map();
                        state = game_state::exploration;
                    }
                }
            }
            else if (state == game_state::exploration) {
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
                        }
                        else if (cell == cell_type::enemy) {
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
            }
            else if (state == game_state::battle) {
                if (current_battle) {
                    current_battle->Update();
                    gfx.RenderBattle(*current_battle);

                    if (current_battle->phase == battle_phase::battle_end && IsKeyPressed(KEY_SPACE)) {
                        if (current_battle->state == battle_state::victory) {
                            game_map.set_empty_cell(ash.get_x(), ash.get_y());
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
            }
            else if (state == game_state::gameover) {
                bool victory = (game_map.count_enemies() == 0);
                gfx.RenderGameOver(victory);

                if (GetKeyPressed() > 0) {
                    break;
                }
            }
        }

        if (current_battle) delete current_battle;
    }

private:
    void InitializeNewPlayerTeam() {
        string types[] = { "fire", "water", "air", "earth" };
        for (int i = 0; i < 3; i++) {
            string r_name = pokemon_names[rand() % pokemon_names.size()];
            string r_type = types[rand() % 4];
            pokemon* p = new pokemon(r_name, r_type);
            ash.add_pokemon_to_team(p);
        }
    }

    void save_game() {
        ofstream out("savegame.txt");
        if (out.is_open()) {
            ash.save(out);
            game_map.save(out);
            out.close();
        }
    }

    bool load_game() {
        ifstream in("savegame.txt");
        if (!in.is_open()) return false;

        ash.load(in);
        game_map.load(in);

        if (!in) {
            in.close();
            return false;
        }
        in.close();
        return true;
    }
};