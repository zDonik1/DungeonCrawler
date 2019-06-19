#include "state_intro.h"
#include "statemanager.h"

State_Intro::State_Intro(StateManager *l_stateManager)
    : BaseState(l_stateManager)
{
}

State_Intro::~State_Intro()
{
}

void State_Intro::onCreate()
{
    sf::Vector2u windowSize = m_stateMgr->getContext()
        ->m_wind->getRenderWindow()->getSize();

    TextureManager* textureMgr = m_stateMgr->getContext()->m_textureManager;
    textureMgr->requireResource("Intro");
    m_introSprite.setTexture(*textureMgr->getResource("Intro"));
    m_introSprite.setOrigin(textureMgr->getResource("Intro")->getSize().x / 2.0f,
                            textureMgr->getResource("Intro")->getSize().y / 2.0f);

    m_introSprite.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

    m_text.setString(sf::String("Press SPACE to continue"));
    m_text.setCharacterSize(15);
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_introSprite.getPosition().x,
                       m_introSprite.getPosition().y + textureMgr->getResource("Intro")->getSize().y / 1.5f);

    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    evMgr->addCallback(StateType::Intro, "Intro_Continue", &State_Intro::resume, this);
    m_stateMgr->getContext()->m_soundManager->playMusic("Electrix", 100.f, true);
}

void State_Intro::onDestroy()
{
    TextureManager *textureMgr = m_stateMgr->getContext()->m_textureManager;
    textureMgr->releaseResource("Intro");

    EventManager* evMgr = m_stateMgr->getContext()->m_eventManager;
    evMgr->removeCallback(StateType::Intro, "Intro_Contrinue");
}

void State_Intro::update(const sf::Time &l_time)
{
}

void State_Intro::draw()
{
    sf::RenderWindow* window = m_stateMgr->getContext()->m_wind->getRenderWindow();
    window->draw(m_introSprite);
    window->draw(m_text);
}

void State_Intro::resume(EventDetails *l_details)
{
    m_stateMgr->switchTo(StateType::MainMenu);
    m_stateMgr->remove(StateType::Intro);
}

void State_Intro::activate()
{
}

void State_Intro::deactivate()
{
}
