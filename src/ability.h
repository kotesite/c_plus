#pragma once

#include <string>

class pokemon;

class ability {
private:
    std::string name_ability;
    int base_damage;

public:
    ability(std::string n, int dmg) : name_ability(std::move(n)), base_damage(dmg) {}

    std::string get_name() const { return name_ability; }
    int get_damage() const { return base_damage; }

    bool Damage(pokemon& target);
};
