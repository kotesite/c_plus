#pragma once

#include <fstream>

#include "pokemon.h"

class player {
private:
    int x, y;
    pokemon* team[3];
    int pokemon_count;

    friend class battle_system;
    friend class GameGraphics;

public:
    player(int start_x, int start_y);
    ~player();

    void add_pokemon_to_team(pokemon* p);
    pokemon* get_active_pokemon();
    void heal_team();

    void set_position(int nx, int ny) { x = nx; y = ny; }
    int get_x() const { return x; }
    int get_y() const { return y; }

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);
};
