#pragma once

#include "s_base.h"
#include "window.h"
#include "c_ui_element.h"
#include "c_health.h"
#include "c_name.h"

class Window;

class S_CharacterUI : public S_Base
{
public:
    S_CharacterUI(SystemManager *l_systemMgr);
    ~S_CharacterUI();

     void update(float l_dt);
     void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
     void notify(const Message &l_message);

     void render(Window *l_wind);

private:
     sf::Sprite m_heartBar;
     sf::Text m_nickname;
     sf::RectangleShape m_nickbg;
     sf::Vector2u m_heartBarSize;
};

