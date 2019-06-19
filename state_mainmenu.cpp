#include "state_mainmenu.h"
#include "statemanager.h"

State_MainMenu::State_MainMenu(StateManager* l_stateManager)
    : BaseState(l_stateManager)
{
}

State_MainMenu::~State_MainMenu()
{
}

void State_MainMenu::onCreate()
{
    setTransparent(true);
    setTranscendent(true);

    GUI_Manager *gui = m_stateMgr->getContext()->m_guiManager;
    gui->loadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
    Window* wnd = m_stateMgr->getContext()->m_wind;
    gui->getInterface(StateType::MainMenu, "MainMenu")->setPosition(sf::Vector2f(wnd->getWindowSize().x / 3.2f, wnd->getWindowSize().y / 3.5f));

    EventManager *eMgr = m_stateMgr->getContext()->m_eventManager;
    eMgr->addCallback(StateType::MainMenu, "MainMenu_Play", &State_MainMenu::play, this);
    eMgr->addCallback(StateType::MainMenu, "MainMenu_Disconnect", &State_MainMenu::disconnect, this);
    eMgr->addCallback(StateType::MainMenu, "MainMenu_Quit", &State_MainMenu::quit, this);
}

void State_MainMenu::onDestroy()
{
    m_stateMgr->getContext()->m_guiManager->removeInterface(StateType::MainMenu, "MainMenu");
    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    GUI_Manager *gui = m_stateMgr->getContext()->m_guiManager;
    gui->removeInterface(StateType::MainMenu, "MainMenu");
    evMgr->removeCallback(StateType::MainMenu, "MainMenu_Play");
    evMgr->removeCallback(StateType::MainMenu, "MainMenu_Disconnect");
    evMgr->removeCallback(StateType::MainMenu, "MainMenu_Quit");
}

void State_MainMenu::activate()
{
    GUI_Interface *menu = m_stateMgr->getContext()->m_guiManager->getInterface(StateType::MainMenu, "MainMenu");
    if (m_stateMgr->hasState(StateType::Game)) {
        menu->getElement("Play")->setText("Resume");
        menu->getElement("Disconnect")->setActive(true);
        menu->getElement("IP")->setActive(false);
        menu->getElement("PORT")->setActive(false);
        menu->getElement("IpLabel")->setActive(false);
        menu->getElement("PortLabel")->setActive(false);
        menu->getElement("NameLabel")->setActive(false);
        menu->getElement("Nickname")->setActive(false);
    }
    else {
        menu->getElement("Play")->setText("Connect");
        menu->getElement("Disconnect")->setActive(false);
        menu->getElement("IP")->setActive(true);
        menu->getElement("PORT")->setActive(true);
        menu->getElement("IpLabel")->setActive(true);
        menu->getElement("PortLabel")->setActive(true);
        menu->getElement("NameLabel")->setActive(true);
        menu->getElement("Nickname")->setActive(true);
    }
}

void State_MainMenu::draw()
{
}

void State_MainMenu::play(EventDetails *l_details)
{
    if (!m_stateMgr->hasState(StateType::Game)) {
        GUI_Interface *menu = m_stateMgr->getContext()->m_guiManager->getInterface(StateType::MainMenu, "MainMenu");
        std::string ip = menu->getElement("IP")->getText();
        PortNumber port =  std::atoi(menu->getElement("PORT")->getText().c_str());
        std::string name = menu->getElement("Nickname")->getText();
        m_stateMgr->getContext()->m_client->setServerInformation(ip, port);
        m_stateMgr->getContext()->m_client->setPlayerName(name);
    }
    m_stateMgr->switchTo(StateType::Game);
}

void State_MainMenu::disconnect(EventDetails *l_details)
{
    m_stateMgr->getContext()->m_client->disconnect();
}

void State_MainMenu::quit(EventDetails *l_details)
{
    m_stateMgr->getContext()->m_wind->close();
}

void State_MainMenu::update(const sf::Time& l_time)
{
}

void State_MainMenu::deactivate()
{
}
