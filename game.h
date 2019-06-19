#pragma once

#include <iostream>
#include "window.h"
#include "eventmanager.h"
#include "statemanager.h"
#include "texturemanager.h"

class Game{
public:
	Game();
	~Game();

    void update();
    void render();
    void lateUpdate();

    sf::Time getElapsed();

    Window* getWindow();

private:
    void restartClock();

    sf::Clock m_clock;
    sf::Time m_elapsed;
	SharedContext m_context;
	Window m_window;
    Client m_client;
    TextureManager m_textureManager;
    FontManager m_fontManager;
    AudioManager m_audioManager;
    SoundManager m_soundManager;
    ClientSystemManager m_systemManager;
    ClientEntityManager m_entityManager;
    GUI_Manager m_guiManager;
    StateManager m_stateManager;
    DebugOverlay m_debugOverlay;
};
