#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "ability.h"

class pokemon {
private:
    std::string name;
    int level;
    int level_evolutions;
    std::string type;
    int hp;
    int max_hp;
    int exp;
    int exp_next_level;
    int attack;

    std::vector<ability*> abilities;

    friend class player;
    friend class battle_system;
    friend bool ability::Damage(pokemon& target);

    static float get_type_koef(const std::string& t);

public:
    pokemon(std::string n, std::string type_pokemon);
    ~pokemon();

    void add_ability(ability* a) { abilities.push_back(a); }
    std::string get_name() const { return name; }

    int get_hp() const { return hp; }
    int get_max_hp() const { return max_hp; }

    const std::vector<ability*>& get_abilities() const { return abilities; }

    void restore_hp() { hp = max_hp; }
    bool is_fail() const { return hp <= 0; }

    void evolution(float koef_type);
    void next_level(int income_exp, float koef_type);

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);
};

std::vector<std::string> load_pokemon_names();
