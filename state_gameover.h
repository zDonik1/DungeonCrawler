#pragma once

#include "basestate.h"

class State_GameOver : public BaseState
{
public:
    State_GameOver(StateManager* l_stateManager);
    ~State_GameOver();

    void onCreate();
    void onDestroy();

    void activate();
    void deactivate();

    void update(const sf::Time& l_time);
    void draw();

private:
    sf::Text m_text;
    float m_elapsed;
};
