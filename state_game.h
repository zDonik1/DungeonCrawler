#pragma once

#include "basestate.h"
#include "map.h"
#include "direction.h"

class State_Game : public BaseState
{
public:
	State_Game(StateManager* l_stateManager);
	~State_Game();

    void onCreate();
    void onDestroy();

    void activate();
    void deactivate();

    void update(const sf::Time &l_time);
    void draw();

    void mainMenu(EventDetails *l_details);
    void pause(EventDetails *l_details);
    void playerMove(EventDetails *l_details);
    void playerAttack(EventDetails *l_details);

    void handlePacket(const PacketID &l_id, sf::Packet &l_packet, Client *l_client);

    // DEBUG
    void toggleDebugOverlay(EventDetails *l_details);

private:
    void updateCamera();

    Map* m_gameMap;
    int m_player;
    Client *m_client;
};
