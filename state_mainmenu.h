#pragma once

#include "basestate.h"
#include "eventmanager.h"

class State_MainMenu : public BaseState
{
public:
	State_MainMenu(StateManager* l_stateManager);
	~State_MainMenu();

    void onCreate();
    void onDestroy();

    void activate();
    void deactivate();

    void update(const sf::Time& l_time);
    void draw();

    void play(EventDetails *l_details);
    void disconnect(EventDetails *l_details);
    void quit(EventDetails *l_details);
};
