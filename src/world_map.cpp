#include "world_map.h"

#include <cstdlib>
#include <queue>
#include <utility>

using namespace std;

bool world_map::can_move(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    if (grid[x][y] == cell_type::wall) return false;
    return true;
}

void world_map::set_empty_cell(int x, int y) {
    if (grid[x][y] == cell_type::enemy) enemy_count--;
    grid[x][y] = cell_type::empty;
}

void world_map::spawn_random_potion() {
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

void world_map::generate_random_map() {
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

        // bfs - ищем все клетки куда можно дойти
        vector<pair<int, int>> reachable_cells;
        vector<vector<bool>> visited(width, vector<bool>(height, false));
        queue<pair<int, int>> q;

        q.push({0, 0});
        visited[0][0] = true;

        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};

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
                        q.push({nx, ny});
                    }
                }
            }
        }

        // мало места - генерим заново
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

void world_map::save(ofstream& out) const {
    out << width << " " << height << " " << enemy_count << "\n";
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            out << static_cast<int>(grid[i][j]) << " ";
        }
        out << "\n";
    }
}

void world_map::load(ifstream& in) {
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
