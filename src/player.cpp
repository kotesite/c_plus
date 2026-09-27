#include "player.h"

using namespace std;

player::player(int start_x, int start_y) : x(start_x), y(start_y), pokemon_count(0) {
    for (int i = 0; i < 3; i++) team[i] = nullptr;
}

player::~player() {
    for (int i = 0; i < pokemon_count; i++) delete team[i];
}

void player::add_pokemon_to_team(pokemon* p) {
    if (pokemon_count < 3) team[pokemon_count++] = p;
}

pokemon* player::get_active_pokemon() {
    for (int i = 0; i < pokemon_count; i++) {
        if (team[i] != nullptr && !team[i]->is_fail()) return team[i];
    }
    return nullptr;
}

void player::heal_team() {
    for (int i = 0; i < pokemon_count; i++) {
        if (team[i] != nullptr) team[i]->restore_hp();
    }
}

void player::save(ofstream& out) const {
    out << x << " " << y << " " << pokemon_count << "\n";
    for (int i = 0; i < pokemon_count; i++) {
        team[i]->save(out);
    }
}

void player::load(ifstream& in) {
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
