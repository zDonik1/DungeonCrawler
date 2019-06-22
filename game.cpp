#include "game.h"

Game::Game()
    : m_window("Chapter 14", sf::Vector2u(640, 480))
    , m_soundManager(&m_audioManager)
    , m_systemManager(&m_context, &m_textureManager, &m_fontManager)
    , m_entityManager(&m_systemManager, &m_textureManager)
    , m_guiManager(m_window.getEventManager(), &m_context)
    , m_stateManager(&m_context)
{
	m_clock.restart();
    srand(static_cast<unsigned int>(time(nullptr)));

    m_systemManager.setEntityManager(&m_entityManager);

    m_client.setServerInformation("localhost", static_cast<PortNumber>(Network::ServerPort));
	m_context.m_wind = &m_window;
    m_context.m_eventManager = m_window.getEventManager();
    m_context.m_textureManager = &m_textureManager;
    m_context.m_fontManager = &m_fontManager;
    m_context.m_audioManager = &m_audioManager;
    m_context.m_soundManager = &m_soundManager;
    m_context.m_systemManager = static_cast<ClientSystemManager*>(&m_systemManager);
    m_context.m_entityManager = static_cast<ClientEntityManager*>(&m_entityManager);
    m_context.m_guiManager = &m_guiManager;
    m_context.m_client = &m_client;

    m_systemManager.getSystem<S_Sound>(System::Sound)->setUp(&m_audioManager, &m_soundManager);

    // DEBUG
    m_systemManager.m_debugOverlay = &m_debugOverlay;
    m_fontManager.requireResource("Main");
    m_stateManager.switchTo(StateType::Intro);
}

Game::~Game()
{
    m_fontManager.releaseResource("Main");
}

sf::Time Game::getElapsed()
{
    return m_clock.getElapsedTime();
}

void Game::restartClock()
{
    m_elapsed = m_clock.restart();
}

Window* Game::getWindow()
{
    return &m_window;
}

void Game::update()
{
    m_stateManager.update(m_elapsed);
    m_client.update(m_elapsed);
    m_guiManager.update(m_elapsed.asSeconds());
    m_soundManager.update(m_elapsed.asSeconds());

    GUI_Event guiEvent;
    while (m_context.m_guiManager->pollEvent(guiEvent)) {
        m_window.getEventManager()->handleEvent(guiEvent);
    }
}

void Game::render()
{
    m_window.beginDraw();

    m_stateManager.draw();

    sf::View currentView = m_window.getRenderWindow()->getView();
    m_window.getRenderWindow()->setView(m_window.getRenderWindow()->getDefaultView());
    m_context.m_guiManager->render(m_window.getRenderWindow());
    m_window.getRenderWindow()->setView(currentView);

    // DEBUG
    m_context.m_debugOverlay.draw(m_window.getRenderWindow());

    m_window.endDraw();
}

void Game::lateUpdate()
{
    m_stateManager.processRequests();
    restartClock();
}
