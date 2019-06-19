#pragma once

#include "window.h"
#include "direction.h"
#include "s_base.h"
#include "c_position.h"
#include "c_drawable.h"

class S_Renderer : public S_Base
{
public:
    S_Renderer(SystemManager *l_systemMgr);
    ~S_Renderer();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    void render(Window *l_wind, unsigned int l_layer);
    void sortDrawables();

private:
    void setSheetDirection(const EntityID &l_entity, const Direction &l_dir);
};
