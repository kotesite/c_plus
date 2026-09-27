#include "battle_system.h"

#include "raylib.h"

using namespace std;

battle_system::battle_system(player& p)
    : state(battle_state::fighting), phase(battle_phase::select_pokemon), p_ref(p), active_my_idx(-1) {
    log_msg1 = "Battle begins!";
    log_msg2 = "Select a Pokemon to attack with [1, 2, 3]";
}

battle_system::~battle_system() {
    for (pokemon* e : enemies) delete e;
}

void battle_system::StartBattle(vector<pokemon*> encountered_enemies) {
    enemies = move(encountered_enemies);
    log_msg1 = "Wild " + enemies[0]->get_name() + " appears!";
    log_msg2 = "Select your Pokemon: keys [1], [2], or [3]";
}

void battle_system::Update() {
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
    } else if (phase == battle_phase::player_anim) {
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
    } else if (phase == battle_phase::enemy_anim) {
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
