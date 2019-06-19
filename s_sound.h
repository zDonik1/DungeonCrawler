#pragma once

#include "s_base.h"
#include "c_soundemitter.h"
#include "c_soundlistener.h"
#include "c_state.h"

class S_Sound : public S_Base
{
public:
    S_Sound(SystemManager *l_systemMgr);
    ~S_Sound();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    void setUp(AudioManager *l_audioManager, SoundManager *l_soundManager);

private:
    sf::Vector3f makeSoundPosition(const sf::Vector2f &l_entityPos, unsigned int l_elevation);
    void emitSound(const EntityID &l_entity, const EntitySound &l_sound, bool l_useID, bool l_relative, int l_checkFrame = -1);

    AudioManager *m_audioManager;
    SoundManager *m_soundManager;
};
