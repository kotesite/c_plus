#pragma once

#include <string>
#include <vector>

#include "graphics.h"
#include "player.h"
#include "world_map.h"

enum class game_state { menu, exploration, battle, gameover };

class game {
private:
    game_state state;
    player ash;
    world_map game_map;
    std::vector<std::string> pokemon_names;

    void InitializeNewPlayerTeam();
    void save_game();
    bool load_game();

public:
    game();

    void MainMenu(GameGraphics& gfx);
    void Run(GameGraphics& gfx);
};
