#pragma once

#include "basestate.h"
#include "eventmanager.h"

class State_Paused : public BaseState
{
public:
	State_Paused(StateManager* l_stateManager);
	~State_Paused();

    void onCreate();
    void onDestroy();

    void activate();
    void deactivate();

    void update(const sf::Time& l_time);
    void draw();

    void unpause(EventDetails* l_details);

private:
	sf::Text m_text;
	sf::RectangleShape m_rect;
};
