#include <iostream>
#include "connect.h"
#include "graphics.h"

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned int>(time(nullptr)));

    // Окно 800x800
    GameGraphics gfx(800, 800, 40);

    game game_r;
    game_r.MainMenu(gfx);

    return 0;
}
//
// Created by ВованЧикс on 07.06.2026.
//