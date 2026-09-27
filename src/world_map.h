#pragma once

#include <fstream>
#include <vector>

enum class cell_type { empty, wall, enemy, potion };

class world_map {
private:
    int width;
    int height;
    std::vector<std::vector<cell_type>> grid;
    int enemy_count;

public:
    world_map() : width(10), height(10), enemy_count(0) {}

    int get_width() const { return width; }
    int get_height() const { return height; }

    bool can_move(int x, int y) const;

    int count_enemies() const { return enemy_count; }
    cell_type get_cell_type(int x, int y) const { return grid[x][y]; }

    void set_empty_cell(int x, int y);
    void set_potion_cell(int x, int y) { grid[x][y] = cell_type::potion; }
    void spawn_random_potion();

    void generate_random_map();

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);
};
