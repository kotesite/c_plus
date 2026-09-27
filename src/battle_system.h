#pragma once

#include <string>
#include <vector>

#include "player.h"
#include "pokemon.h"

enum class battle_state { fighting, victory, defeat };
enum class battle_phase { select_pokemon, player_anim, enemy_anim, battle_end };

class battle_system {
public:
    battle_state state;
    battle_phase phase;
    std::vector<pokemon*> enemies;
    player& p_ref;
    std::string log_msg1;
    std::string log_msg2;
    int active_my_idx;

    battle_system(player& p);
    ~battle_system();

    void StartBattle(std::vector<pokemon*> encountered_enemies);
    void Update();
};
