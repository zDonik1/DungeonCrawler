#include "state_gameover.h"
#include "statemanager.h"

State_GameOver::State_GameOver(StateManager* l_stateManager) : BaseState(l_stateManager)
{
}

State_GameOver::~State_GameOver()
{
}

void State_GameOver::onCreate()
{
    m_elapsed = 0;
    m_text.setCharacterSize(24);
    m_text.setString("You beat the game! Congratulations");
    m_text.setFillColor(sf::Color::White);
    m_text.setOrigin(m_text.getLocalBounds().width / 2, m_text.getLocalBounds().height / 2);
    m_text.setPosition(400, 300);

    m_stateMgr->remove(StateType::Game);
}

void State_GameOver::onDestroy()
{
}

void State_GameOver::activate()
{
}

void State_GameOver::deactivate()
{
}

void State_GameOver::update(const sf::Time &l_time)
{
    m_elapsed += l_time.asSeconds();
    if (m_elapsed >= 5.0f)
    {
        m_stateMgr->remove(StateType::GameOver);
        m_stateMgr->switchTo(StateType::MainMenu);
    }
}

void State_GameOver::draw()
{
    sf::RenderWindow* window = m_stateMgr->getContext()->m_wind->getRenderWindow();
    window->draw(m_text);
}
