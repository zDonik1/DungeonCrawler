#pragma once

#include "systemmanager.h"
#include "window.h"
#include "s_renderer.h"
#include "s_movement.h"
#include "s_collision.h"
#include "s_state.h"
#include "s_control.h"
#include "s_sound.h"
#include "s_sheetanimation.h"
#include "s_network.h"
#include "s_characterui.h"
#include "fontmanager.h"
#include "debugoverlay.h"

class ClientSystemManager : public SystemManager
{
public:
    ClientSystemManager(TextureManager *l_textureMgr, FontManager *l_fontMgr);
    ~ClientSystemManager();

    TextureManager *getTextureManager();
    FontManager *getFontManager();

    void draw(Window *l_wind, unsigned int l_elevation);

    // DEBUG
    DebugOverlay *m_debugOverlay;

private:
    TextureManager *m_textureMgr;
    FontManager *m_fontMgr;
};

