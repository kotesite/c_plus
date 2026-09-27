#include "pokemon.h"

#include <cmath>

using namespace std;

float pokemon::get_type_koef(const string& t) {
    if (t == "fire") return 2.8f;
    if (t == "water") return 2.6f;
    if (t == "air") return 2.5f;
    if (t == "earth") return 2.7f;
    return 1.0f;
}

pokemon::pokemon(string n, string type_pokemon)
    : name(move(n)), level(1), level_evolutions(0), type(type_pokemon),
      hp(static_cast<int>(100 * get_type_koef(type_pokemon))),
      max_hp(static_cast<int>(100 * get_type_koef(type_pokemon))),
      exp(0), exp_next_level(100), attack(10) {
    add_ability(new ability("Basic Strike", 20));
}

pokemon::~pokemon() {
    for (ability* a : abilities) delete a;
}

void pokemon::evolution(float koef_type) {
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

void pokemon::next_level(int income_exp, float koef_type) {
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

void pokemon::save(ofstream& out) const {
    out << name << " " << level << " " << level_evolutions << " " << type << " "
        << hp << " " << max_hp << " " << exp << " " << exp_next_level << " " << attack << "\n";
}

void pokemon::load(ifstream& in) {
    in >> name >> level >> level_evolutions >> type >> hp >> max_hp >> exp >> exp_next_level >> attack;
}

vector<string> load_pokemon_names() {
    vector<string> names;
    ifstream file("data/pokemon_names.txt");
    if (!file.is_open()) {
        names = {"Pikachu", "Charizard", "Bulbasaur", "Squirtle", "Zubat",
                 "Geodude", "Eevee",     "Snorlax",   "Mewtwo",   "Abra"};
    } else {
        string name;
        while (file >> name) {
            names.push_back(name);
        }
    }
    if (names.empty()) names = {"MissingNo"};
    return names;
}
