#include "game.h"

int main()
{
    Game game;

    while (!game.getWindow()->isDone()) {
        game.update();
        game.render();
        game.lateUpdate();
    }

    return 0;
}
