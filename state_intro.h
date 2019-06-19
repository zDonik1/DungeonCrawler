#pragma once

#include "basestate.h"
#include "eventmanager.h"

class State_Intro : public BaseState
{
public:
    State_Intro(StateManager* l_stateManager);
    ~State_Intro();

    void onCreate();
    void onDestroy();

    void activate();
    void deactivate();

    void update(const sf::Time& l_time);
    void draw();

    void resume(EventDetails* l_details);

private:
    sf::Sprite m_introSprite;
    sf::Text m_text;
};
