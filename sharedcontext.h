#pragma once

#include "window.h"
#include "eventmanager.h"
#include "texturemanager.h"
#include "fontmanager.h"
#include "soundmanager.h"
#include "gui_manager.h"
#include "clientsystemmanager.h"
#include "cliententitymanager.h"
#include "client.h"
#include "debugoverlay.h"

class Map;

// SharedContext structure - holds pointers to important assets of the game
struct SharedContext
{
    SharedContext()
        : m_wind(nullptr)
        , m_client(nullptr)
        , m_eventManager(nullptr)
        , m_textureManager(nullptr)
        , m_fontManager(nullptr)
        , m_audioManager(nullptr)
        , m_soundManager(nullptr)
        , m_systemManager(nullptr)
        , m_entityManager(nullptr)
        , m_gameMap(nullptr)
        , m_guiManager(nullptr)
    {
    }

    Window *m_wind;
    Client *m_client;
    EventManager *m_eventManager;
    TextureManager *m_textureManager;
    FontManager *m_fontManager;
    AudioManager *m_audioManager;
    SoundManager *m_soundManager;
    ClientSystemManager *m_systemManager;
    ClientEntityManager *m_entityManager;
    Map *m_gameMap;
    GUI_Manager *m_guiManager;
    DebugOverlay m_debugOverlay;
};
