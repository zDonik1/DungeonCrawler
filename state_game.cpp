#include "state_game.h"
#include "statemanager.h"
#include "entitysnapshot.h"

State_Game::State_Game(StateManager* l_stateManager)
    : BaseState(l_stateManager)
    , m_gameMap(nullptr)
    , m_player(-1)
    , m_client(m_stateMgr->getContext()->m_client)
{
}

State_Game::~State_Game()
{
}

void State_Game::onCreate()
{
    m_client->setup(&State_Game::handlePacket, this);
    if (m_client->connect()) {
        m_stateMgr->getContext()->m_systemManager->getSystem<S_Network>(System::Network)->setClient(m_client);

        EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
        evMgr->addCallback(StateType::Game, "Key_Escape", &State_Game::mainMenu, this);
        evMgr->addCallback(StateType::Game, "Key_O", &State_Game::toggleDebugOverlay, this);
        evMgr->addCallback(StateType::Game, "Key_P", &State_Game::pause, this);
        evMgr->addCallback(StateType::Game, "Player_MoveLeft", &State_Game::playerMove, this);
        evMgr->addCallback(StateType::Game, "Player_MoveRight", &State_Game::playerMove, this);
        evMgr->addCallback(StateType::Game, "Player_MoveUp", &State_Game::playerMove, this);
        evMgr->addCallback(StateType::Game, "Player_MoveDown", &State_Game::playerMove, this);
        evMgr->addCallback(StateType::Game, "Player_Attack", &State_Game::playerAttack, this);

        sf::Vector2u size = m_stateMgr->getContext()->m_wind->getWindowSize();
        m_view.setSize(size.x, size.y);
        m_view.setCenter(size.x / 2, size.y / 2);
        m_view.zoom(0.6f);
        m_stateMgr->getContext()->m_wind->getRenderWindow()->setView(m_view);

        m_gameMap = new Map(m_stateMgr->getContext());
        m_gameMap->loadMap("media/Maps/map1.map");

        m_stateMgr->getContext()->m_systemManager->getSystem<S_Collision>(System::Collision)->setMap(m_gameMap);
        m_stateMgr->getContext()->m_systemManager->getSystem<S_Movement>(System::Movement)->setMap(m_gameMap);

        m_stateMgr->getContext()->m_soundManager->playMusic("TownTheme", 50.f, true);
    }
    else {
        std::cout << "Failed to connect to the game server!" << std::endl;
        m_stateMgr->remove(StateType::Game);
        m_stateMgr->switchTo(StateType::MainMenu);
    }
}

void State_Game::onDestroy()
{
    m_client->disconnect();
    m_client->unregisterPacketHandler();
    S_Network *net = m_stateMgr->getContext()->m_systemManager->getSystem<S_Network>(System::Network);
    net->clearSnapshots();
    net->setClient(nullptr);
    net->setPlayerID((int)Network::NullID);

    m_stateMgr->getContext()->m_guiManager->removeInterface(StateType::Game, "Test");

    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    evMgr->removeCallback(StateType::Game, "Key_Escape");
    evMgr->removeCallback(StateType::Game, "Key_O");
    evMgr->removeCallback(StateType::Game, "Key_P");
    evMgr->removeCallback(StateType::Game, "Player_MoveLeft");
    evMgr->removeCallback(StateType::Game, "Player_MoveRight");
    evMgr->removeCallback(StateType::Game, "Player_MoveUp");
    evMgr->removeCallback(StateType::Game, "Player_MoveDown");
    evMgr->removeCallback(StateType::Game, "Player_Attack");

    if (m_gameMap) {
        delete m_gameMap;
        m_gameMap = nullptr;
    }
}

void State_Game::update(const sf::Time& l_time)
{
    if (!m_client->isConnected()) {
        m_stateMgr->remove(StateType::Game);
        m_stateMgr->switchTo(StateType::MainMenu);
        return;
    }

    SharedContext *context = m_stateMgr->getContext();
    updateCamera();
    m_gameMap->update(l_time.asSeconds());

    {
        sf::Lock lock(m_client->getMutex());
        context->m_systemManager->update(l_time.asSeconds());
    }
}

void State_Game::draw()
{
    if (!m_gameMap) {
        return;
    }

    sf::Lock lock(m_client->getMutex());
    for (int i = 0; i < Sheet::Num_Layers; ++i) {
        m_gameMap->draw(i);
        m_stateMgr->getContext()->m_systemManager->draw(m_stateMgr->getContext()->m_wind, i);
    }
}

void State_Game::mainMenu(EventDetails* l_details)
{
    m_stateMgr->switchTo(StateType::MainMenu);
}

void State_Game::pause(EventDetails* l_details)
{
    m_stateMgr->switchTo(StateType::Paused);
}

void State_Game::playerMove(EventDetails *l_details)
{
    Message msg((MessageType)EntityMessage::Move);
    if (l_details->m_name == "Player_MoveLeft") {
        msg.m_int = (int)Direction::Left;
    }
    else if (l_details->m_name == "Player_MoveRight") {
        msg.m_int = (int)Direction::Right;
    }
    else if (l_details->m_name == "Player_MoveUp") {
        msg.m_int = (int)Direction::Up;
    }
    else if (l_details->m_name == "Player_MoveDown") {
        msg.m_int = (int)Direction::Down;
    }

    msg.m_receiver = m_player;
    m_stateMgr->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
}

void State_Game::playerAttack(EventDetails *l_details)
{
    Message msg((MessageType)EntityMessage::Attack);
    msg.m_receiver = m_player;
    m_stateMgr->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
}

void State_Game::toggleDebugOverlay(EventDetails *l_details)
{
    m_stateMgr->getContext()->m_debugOverlay.setDebug(!m_stateMgr->getContext()->m_debugOverlay.getDebug());
}

void State_Game::handlePacket(const PacketID &l_id, sf::Packet &l_packet, Client *l_client)
{
    ClientEntityManager *emgr = (ClientEntityManager*)m_stateMgr->getContext()->m_entityManager;
    PacketType type = (PacketType)l_id;
    if (type == PacketType::Connect) {
        sf::Int32 eid;
        sf::Vector2f pos;
        if (!(l_packet >> eid) || !(l_packet >> pos.x) || !(l_packet >> pos.y)) {
            std::cout << "Faulty CONNECT response!" << std::endl;
            return;
        }

        std::cout << "Adding entity: " << eid << std::endl;
        m_client->getMutex().lock();
        emgr->addEntity("Player", eid);
        emgr->getComponent<C_Position>(eid, Component::Position)->setPosition(pos);
        m_client->getMutex().unlock();
        m_player = eid;
        m_stateMgr->getContext()->m_systemManager->getSystem<S_Network>(System::Network)->setPlayerID(m_player);
        emgr->addComponent(eid, Component::SoundListener);
        return;
    }

    if (!m_client->isConnected()) {
        return;
    }

    switch (type) {
    case PacketType::Snapshot:
    {
        sf::Int32 entityCount = 0;
        if (!(l_packet >> entityCount)) {
            std::cout << "Snapshot extraction failed." << std::endl;
            return;
        }

        sf::Lock lock(m_client->getMutex());
        sf::Int32 t = m_client->getTime().asMilliseconds();
        for (unsigned int i = 0; i < entityCount; ++i) {
            sf::Int32 eid;
            EntitySnapshot snapshot;
            if (!(l_packet >> eid) || !(l_packet >> snapshot)) {
                std::cout << "Snapshot extraction failed." << std::endl;
                return;
            }

            m_stateMgr->getContext()->m_systemManager->getSystem<S_Network>(System::Network)->addSnapshot(eid, t, snapshot);
        }

        break;
    }

    case PacketType::Disconnect:
    {
        m_stateMgr->remove(StateType::Game);
        m_stateMgr->switchTo(StateType::MainMenu);
        std::cout << "Disconnect by server!" << std::endl;
        break;
    }

    case PacketType::Hurt:
    {
        EntityID id;
        if (!(l_packet >> id)) {
            return;
        }

        Message msg((MessageType)EntityMessage::Hurt);
        msg.m_receiver = id;
        m_stateMgr->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
        break;
    }
    }
}

void State_Game::updateCamera()
{
    if (m_player == static_cast<int>(Network::NullID)) {
        return;
    }
    SharedContext *context = m_stateMgr->getContext();
    C_Position *pos = m_stateMgr->getContext()->m_entityManager->getComponent<C_Position>(m_player, Component::Position);

    m_view.setCenter(pos->getPosition());
    context->m_wind->getRenderWindow()->setView(m_view);

    sf::FloatRect viewSpace = context->m_wind->getViewSpace();
    if (viewSpace.left <= 0) {
        m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
        context->m_wind->getRenderWindow()->setView(m_view);
    }
    else if (viewSpace.left + viewSpace.width > (m_gameMap->getMapSize().x) * Sheet::Tile_Size) {
        m_view.setCenter((m_gameMap->getMapSize().x * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
        context->m_wind->getRenderWindow()->setView(m_view);
    }

    if (viewSpace.top <= 0) {
        m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
        context->m_wind->getRenderWindow()->setView(m_view);
    }
    else if (viewSpace.top + viewSpace.height > m_gameMap->getMapSize().y * Sheet::Tile_Size) {
        m_view.setCenter(m_view.getCenter().x, (m_gameMap->getMapSize().y * Sheet::Tile_Size) - (viewSpace.height / 2));
        context->m_wind->getRenderWindow()->setView(m_view);
    }

    // DEBUG
    if (context->m_debugOverlay.getDebug()){
        sf::Text* d_pos = new sf::Text();
        d_pos->setFont(*m_stateMgr->getContext()->m_fontManager->getResource("Main"));
        d_pos->setString("Player position: " + std::to_string((long long)pos->getPosition().x) + " " + std::to_string((long long)pos->getPosition().y));
        d_pos->setCharacterSize(9);
        d_pos->setPosition(context->m_wind->getRenderWindow()->getView().getCenter() - sf::Vector2f(
                               context->m_wind->getRenderWindow()->getView().getSize().x / 2,
                               context->m_wind->getRenderWindow()->getView().getSize().y / 2)
                           );

        context->m_debugOverlay.add(d_pos);
    }
}

void State_Game::activate()
{

}
void State_Game::deactivate()
{

}
