#pragma once

#include "s_base.h"
#include "c_state.h"

class S_SheetAnimation : public S_Base
{
public:
    S_SheetAnimation(SystemManager *l_systemMgr);
    ~S_SheetAnimation();

    void update(float l_dt);
    void notify(const Message &l_message);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);

private:
    void changeAnimation(const EntityID &l_entity, const std::string &l_anim, bool l_play, bool l_loop);
};
