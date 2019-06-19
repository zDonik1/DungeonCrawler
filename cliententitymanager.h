#pragma once

#include "entitymanager.h"
#include "c_collidable.h"
#include "c_controller.h"
#include "c_movable.h"
#include "c_state.h"
#include "c_position.h"
#include "c_spritesheet.h"
#include "c_soundemitter.h"
#include "c_soundlistener.h"
#include "c_client.h"
#include "c_health.h"
#include "c_name.h"
#include "c_ui_element.h"
#include "texturemanager.h"

class SystemManager;

class ClientEntityManager : public EntityManager
{
public:
    ClientEntityManager(SystemManager *l_sysMgr, TextureManager *l_textureMgr);
    ~ClientEntityManager();

    int addEntity(const std::string &l_entityFile, int l_id = -1);

private:
    TextureManager *m_textureManager;
};

