#include "state_paused.h"
#include "statemanager.h"

State_Paused::State_Paused(StateManager* l_stateManager) : BaseState(l_stateManager) {}

State_Paused::~State_Paused() {}

void State_Paused::onCreate()
{
    setTransparent(true); // Set our transparency flag.
	m_text.setString(sf::String("PAUSED"));
	m_text.setCharacterSize(14);
	m_text.setStyle(sf::Text::Bold);

    sf::Vector2u windowSize = m_stateMgr->getContext()->m_wind->getRenderWindow()->getSize();

	sf::FloatRect textRect = m_text.getLocalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

	m_rect.setSize(sf::Vector2f(windowSize));
	m_rect.setPosition(0,0);
	m_rect.setFillColor(sf::Color(0,0,0,150));

    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    evMgr->addCallback(StateType::Paused,"Key_P",&State_Paused::unpause,this);
}

void State_Paused::onDestroy()
{
    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    evMgr->removeCallback(StateType::Paused,"Key_P");
}

void State_Paused::draw()
{
    sf::RenderWindow* wind = m_stateMgr->getContext()->m_wind->getRenderWindow();
	wind->draw(m_rect);
	wind->draw(m_text);
}

void State_Paused::unpause(EventDetails* l_details)
{
    m_stateMgr->switchTo(StateType::Game);
}

void State_Paused::activate() {}
void State_Paused::deactivate() {}
void State_Paused::update(const sf::Time& l_time) {}
