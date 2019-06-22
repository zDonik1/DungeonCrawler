#include "game.h"

#define SLEEP_TIME 1
#define UPDATE_TIMESTEP 20

int main()
{
    Game game;
    sf::Clock clock;
    sf::Time elapsed_forUpdate = clock.restart();

    while (!game.getWindow()->isDone()) {
        game.getWindow()->update();

        if (elapsed_forUpdate.asMilliseconds() >= UPDATE_TIMESTEP) {
            game.update();
            elapsed_forUpdate -= sf::milliseconds(UPDATE_TIMESTEP);
        }

        game.render();
        game.lateUpdate();

        elapsed_forUpdate += clock.getElapsedTime();
        sf::sleep(sf::milliseconds(SLEEP_TIME));
    }

    return 0;
}
