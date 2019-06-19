#include "s_sound.h"
#include "systemmanager.h"
#include "map.h"

S_Sound::S_Sound(SystemManager *l_systemMgr)
    : S_Base(System::Sound, l_systemMgr)
    , m_audioManager(nullptr)
    , m_soundManager(nullptr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::SoundEmitter);
    m_requiredComponents.push_back(req);

    req.clearBit((unsigned int)Component::SoundEmitter);
    req.turnOnBit((unsigned int)Component::SoundListener);
    m_requiredComponents.push_back(req);

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Direction_Changed, this);
    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Frame_Change, this);
}

S_Sound::~S_Sound()
{
}

void S_Sound::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Position *c_pos = entities->getComponent<C_Position>(entity, Component::Position);
        sf::Vector2f position = c_pos->getPosition();
        unsigned int elevation = c_pos->getElevation();
        bool isListener = entities->hasComponent(entity, Component::SoundListener);
        if (isListener) {
            sf::Listener::setPosition(makeSoundPosition(position, elevation));
        }

        if (!entities->hasComponent(entity, Component::SoundEmitter)) {
            continue;
        }

        C_SoundEmitter *c_snd = entities->getComponent<C_SoundEmitter>(entity, Component::SoundEmitter);
        if (c_snd->getSoundID() == -1) {
            continue;
        }

        if (!isListener) {
            if (!m_soundManager->setPosition(c_snd->getSoundID(), makeSoundPosition(position, elevation))) {
                c_snd->setSoundID(-1);
            }
        }
        else {
            if (!m_soundManager->isPlaying(c_snd->getSoundID())) {
                c_snd->setSoundID(-1);
            }
        }
    }
}

void S_Sound::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_Sound::notify(const Message &l_message)
{
    if (!hasEntity(l_message.m_receiver)) {
        return;
    }

    EntityManager *entities = m_systemManager->getEntityManager();
    bool isListener = entities->hasComponent(l_message.m_receiver, Component::SoundListener);
    EntityMessage m = (EntityMessage)l_message.m_type;
    switch (m) {
    case EntityMessage::Direction_Changed:
    {
        if (!isListener) {
            return;
        }

        Direction dir = (Direction)l_message.m_int;
        switch (dir) {
        case Direction::Up:
            sf::Listener::setDirection(0, 0, -1);
            break;

        case Direction::Down:
            sf::Listener::setDirection(0, 0, 1);
            break;

        case Direction::Left:
            sf::Listener::setDirection(-1, 0, 0);
            break;

        case Direction::Right:
            sf::Listener::setDirection(1, 0, 0);
            break;
        }
    }
        break;

    case EntityMessage::Frame_Change:
        if (!entities->hasComponent(l_message.m_receiver, Component::SoundEmitter)) {
            return;
        }

        EntityState state = entities->getComponent<C_State>(l_message.m_receiver, Component::State)->getState();
        EntitySound sound = EntitySound::None;
        if (state == EntityState::Walking) {
            sound = EntitySound::Footstep;
        }
        else if (state == EntityState::Attacking) {
            sound = EntitySound::Attack;
        }
        else if (state == EntityState::Hurt) {
            sound = EntitySound::Hurt;
        }
        else if (state == EntityState::Dying) {
            sound = EntitySound::Death;
        }

        if (sound == EntitySound::None) {
            return;
        }

        emitSound(l_message.m_receiver, sound, false, isListener, l_message.m_int);
        break;
    }
}

void S_Sound::setUp(AudioManager *l_audioManager, SoundManager *l_soundManager)
{
    m_audioManager = l_audioManager;
    m_soundManager = l_soundManager;
}

sf::Vector3f S_Sound::makeSoundPosition(const sf::Vector2f &l_entityPos, unsigned int l_elevation)
{
    return sf::Vector3f(l_entityPos.x, l_elevation * Sheet::Tile_Size, l_entityPos.y);
}

void S_Sound::emitSound(const EntityID &l_entity, const EntitySound &l_sound, bool l_useID, bool l_relative, int l_checkFrame)
{
    if (!hasEntity(l_entity)) {
        return;
    }

    if (!m_systemManager->getEntityManager()->hasComponent(l_entity, Component::SoundEmitter)) {
        return;
    }

    EntityManager *entities = m_systemManager->getEntityManager();
    C_SoundEmitter *c_snd = entities->getComponent<C_SoundEmitter>(l_entity, Component::SoundEmitter);
    if (c_snd->getSoundID() != -1 && l_useID) {
        return;
    }

    if (l_checkFrame != -1 && !c_snd->isSoundFrame(l_sound, l_checkFrame)) {
        return;
    }

    C_Position *c_pos = entities->getComponent<C_Position>(l_entity, Component::Position);
    sf::Vector3f pos = (l_relative ? sf::Vector3f(0.f, 0.f, 0.f) : makeSoundPosition(c_pos->getPosition(), c_pos->getElevation()));
    if (l_useID) {
        c_snd->setSoundID(m_soundManager->play(c_snd->getSound(l_sound), pos));
    }
    else {
        m_soundManager->play(c_snd->getSound(l_sound), pos, false, l_relative);
    }
}
