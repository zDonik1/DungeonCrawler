#include "s_sheetanimation.h"
#include "systemmanager.h"
#include "c_spritesheet.h"

S_SheetAnimation::S_SheetAnimation(SystemManager *l_systemMgr)
    : S_Base(System::SheetAnimation, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::SpriteSheet);
    req.turnOnBit((unsigned int)Component::State);
    m_requiredComponents.push_back(req);

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::State_Changed, this);
}

S_SheetAnimation::~S_SheetAnimation()
{
}

void S_SheetAnimation::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_SpriteSheet *sheet = entities->getComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
        C_State *state = entities->getComponent<C_State>(entity, Component::State);

        sheet->getSpriteSheet()->update(l_dt);

        const std::string &animName = sheet->getSpriteSheet()->getCurrentAnim()->getName();
        if (animName == "Attack") {
            if (!sheet->getSpriteSheet()->getCurrentAnim()->isPlaying()) {
                Message msg((MessageType)EntityMessage::Switch_State);
                msg.m_receiver = entity;
                msg.m_int = (int)EntityState::Idle;
                m_systemManager->getMessageHandler()->dispatch(msg);
            }
            else if (sheet->getSpriteSheet()->getCurrentAnim()->isInAction()) {
                Message msg((MessageType)EntityMessage::Attack_Action);
                msg.m_sender = entity;
                m_systemManager->getMessageHandler()->dispatch(msg);
            }
        }
        else if (animName == "Death" && !sheet->getSpriteSheet()->getCurrentAnim()->isPlaying()) {
            Message msg((MessageType)EntityMessage::Dead);
            msg.m_receiver = entity;
            m_systemManager->getMessageHandler()->dispatch(msg);
        }

        if (sheet->getSpriteSheet()->getCurrentAnim()->checkMoved()) {
            int frame = sheet->getSpriteSheet()->getCurrentAnim()->getFrame();
            Message msg((MessageType)EntityMessage::Frame_Change);
            msg.m_receiver = entity;
            msg.m_int = frame;
            m_systemManager->getMessageHandler()->dispatch(msg);
        }
    }
}

void S_SheetAnimation::notify(const Message &l_message)
{
    if (hasEntity(l_message.m_receiver)) {
        EntityMessage m = (EntityMessage)l_message.m_type;
        switch (m) {
        case EntityMessage::State_Changed:
        {
            EntityState s = (EntityState)l_message.m_int;
            switch (s) {
            case EntityState::Idle:
                changeAnimation(l_message.m_receiver, "Idle", true, true);
                break;

            case EntityState::Walking:
                changeAnimation(l_message.m_receiver, "Walk", true, true);
                break;

            case EntityState::Attacking:
                changeAnimation(l_message.m_receiver, "Attack", true, false);
                break;

            case EntityState::Hurt:
                break;

            case EntityState::Dying:
                changeAnimation(l_message.m_receiver, "Death", true, false);
                break;
            }
        }
            break;
        }
    }
}

void S_SheetAnimation::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
    if (!hasEntity(l_entity)) {
        return;
    }

    if (l_event != EntityEvent::Colliding_X && l_event != EntityEvent::Colliding_Y) {
        return;
    }

    C_SpriteSheet *sheet = m_systemManager->getEntityManager()->getComponent<C_SpriteSheet>(l_entity,Component::SpriteSheet);
    if (sheet->getSpriteSheet()->getCurrentAnim()->getName() != "Walk") {
        return;
    }

    sheet->getSpriteSheet()->getCurrentAnim()->setFrame(0);
}

void S_SheetAnimation::changeAnimation(const EntityID &l_entity, const std::string &l_anim, bool l_play, bool l_loop)
{
    C_SpriteSheet *sheet = m_systemManager->getEntityManager()->getComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    sheet->getSpriteSheet()->setAnimation(l_anim, l_play, l_loop);
}
