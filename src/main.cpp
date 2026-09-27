#include <clocale>
#include <cstdlib>
#include <ctime>

#include "game.h"
#include "graphics.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned int>(time(nullptr)));

    // окно 800x800
    GameGraphics gfx(800, 800, 40);

    game game_r;
    game_r.MainMenu(gfx);

    return 0;
}
