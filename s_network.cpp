#include "s_network.h"
#include "systemmanager.h"
#include "s_renderer.h"

S_Network::S_Network(SystemManager *l_systemMgr)
    : S_Base(System::Network, l_systemMgr)
    , m_client(nullptr)
{
    Bitmask req;
    req.turnOnBit(static_cast<unsigned int>(Component::Client));
    m_requiredComponents.push_back(req);

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Move, this);
    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Attack, this);
    m_playerUpdateTimer = sf::milliseconds(0);
}

S_Network::~S_Network()
{
}

void S_Network::update(float l_dt)
{
    if (!m_client) {
        return;
    }

    sf::Lock lock(m_client->getMutex());
    m_playerUpdateTimer += sf::seconds(l_dt);
    if (m_playerUpdateTimer.asMilliseconds() >= PLAYER_UPDATE_INTERVAL) {
        sendPlayerOutgoing();
        m_playerUpdateTimer = sf::milliseconds(0);
    }

    performInterpolation();
}

void S_Network::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_Network::notify(const Message &l_message)
{
    if (!hasEntity(static_cast<EntityID>(l_message.m_receiver)) || static_cast<EntityID>(l_message.m_receiver) != m_player) {
        return;
    }

    if (l_message.m_type == static_cast<MessageType>(EntityMessage::Attack) && m_outgoing.find(EntityMessage::Attack) != m_outgoing.end()) {
        return;
    }

    m_outgoing[static_cast<EntityMessage>(l_message.m_type)].emplace_back(l_message);
}

void S_Network::setClient(Client *l_client)
{
    m_client = l_client;
}

void S_Network::setPlayerID(const EntityID &l_entity)
{
    m_player = l_entity;
}

void S_Network::addSnapshot(const EntityID &l_entity, const sf::Int32 &l_timestamp, EntitySnapshot &l_snapshot)
{
    sf::Lock lock(m_client->getMutex());
    auto i = m_entitySnapshots.emplace(l_timestamp, SnapshotDetails());
    i.first->second.m_snapshots.emplace(l_entity, l_snapshot);
}

void S_Network::sendPlayerOutgoing()
{
    sf::Int32 p_x = 0;
    sf::Int32 p_y = 0;
    sf::Int8 p_a = 0;

    for (auto &itr : m_outgoing) {
        if (itr.first == EntityMessage::Move) {
            sf::Int32 x = 0;
            sf::Int32 y = 0;
            for (auto &message : itr.second) {
                if (message.m_int == static_cast<int>(Direction::Up)) {
                    --y;
                }
                else if (message.m_int == static_cast<int>(Direction::Down)) {
                    ++y;
                }
                else if (message.m_int == static_cast<int>(Direction::Left)) {
                    --x;
                }
                else if (message.m_int == static_cast<int>(Direction::Right)) {
                    ++x;
                }
            }

            if (!x && !y) {
                continue;
            }

            p_x = x;
            p_y = y;
        }
        else if (itr.first == EntityMessage::Attack) {
            p_a = 1;
        }
    }

    sf::Packet packet;
    stampPacket(PacketType::Player_Update, packet);
    packet << sf::Int8(EntityMessage::Move) << p_x << p_y << sf::Int8(Network::PlayerUpdateDelim);
    packet << sf::Int8(EntityMessage::Attack) << p_a << sf::Int8(Network::PlayerUpdateDelim);
    m_client->send(packet);
    m_outgoing.clear();
}

void S_Network::clearSnapshots()
{
    m_entitySnapshots.clear();
}

void S_Network::applyEntitySnapshot(const EntityID &l_entity, const EntitySnapshot &l_snapshot, bool l_applyPhysics)
{
    ClientEntityManager *entities = static_cast<ClientEntityManager*>(m_systemManager->getEntityManager());
    C_Position *position = nullptr;
    C_Movable *movable = nullptr;
    S_Movement *movement_s = nullptr;
    S_State *state_s = nullptr;
    C_Health *health = nullptr;
    C_Name *name = nullptr;
    sf::Lock lock(m_client->getMutex());
    if ((position = entities->getComponent<C_Position>(l_entity, Component::Position))) {
        position->setPosition(l_snapshot.m_position);
        position->setElevation(static_cast<unsigned int>(l_snapshot.m_elevation));
    }
    if (l_applyPhysics) {
        if ((movable = entities->getComponent<C_Movable>(l_entity, Component::Movable))) {
            movable->setVelocity(l_snapshot.m_velocity);
            movable->setAcceleration(l_snapshot.m_acceleration);
        }
    }
    if ((movement_s = m_systemManager->getSystem<S_Movement>(System::Movement))) {
        movement_s->setDirection(l_entity, static_cast<Direction>(l_snapshot.m_direction));
    }
    if ((state_s = m_systemManager->getSystem<S_State>(System::State))) {
        state_s->changeState(l_entity, static_cast<EntityState>(l_snapshot.m_state), true);
    }
    if ((health = entities->getComponent<C_Health>(l_entity, Component::Health))) {
        health->setHealth(l_snapshot.m_health);
    }
    if ((name = entities->getComponent<C_Name>(l_entity, Component::Name))) {
        name->setName(l_snapshot.m_name);
    }
}

void S_Network::performInterpolation()
{
    if (m_entitySnapshots.empty()) {
        return;
    }

    ClientEntityManager *entities = static_cast<ClientEntityManager*>(m_systemManager->getEntityManager());
    sf::Time t = m_client->getTime();
    auto itr = ++m_entitySnapshots.begin();
    while (itr != m_entitySnapshots.end()) {
        if (m_entitySnapshots.begin()->first <= t.asMilliseconds() - NET_RENDER_DELAY
                && itr->first >= t.asMilliseconds() - NET_RENDER_DELAY)
        {
            auto snapshot1 = m_entitySnapshots.begin();
            auto snapshot2 = itr;
            bool sortDrawables = false;
            for (auto snap = snapshot1->second.m_snapshots.begin(); snap != snapshot1->second.m_snapshots.end();) {
                if (!entities->hasEntity(snap->first)) {
                    if (entities->addEntity(snap->second.m_type, static_cast<int>(snap->first))
                            == static_cast<int>(Network::NullID))
                    {
                        std::cout << "Failed adding entity type: " << snap->second.m_type << std::endl;
                        continue;
                    }

                    applyEntitySnapshot(snap->first, snap->second, true);
                    ++snap;
                    continue;
                }

                auto snap2 = snapshot2->second.m_snapshots.find(snap->first);
                if (snap2 == snapshot2->second.m_snapshots.end()) {
                    sf::Lock lock(m_client->getMutex());
                    entities->removeEntity(snap->first);
                    snap = snapshot1->second.m_snapshots.erase(snap);
                    continue;
                }

                EntitySnapshot i_snapshot;
                interpolateSnapshot(snap->second, snapshot1->first, snap2->second, snapshot2->first,
                                    i_snapshot, t.asMilliseconds() - NET_RENDER_DELAY);
                applyEntitySnapshot(snap->first, i_snapshot, true);
                if (!compareSnapshots(snap->second, snap2->second, true, false, false)) {
                    sortDrawables = true;
                }
                ++snap;
            }

            if (sortDrawables) {
                m_systemManager->getSystem<S_Renderer>(System::Renderer)->sortDrawables();
            }

            return;
        }

        m_entitySnapshots.erase(m_entitySnapshots.begin());
        itr = ++m_entitySnapshots.begin();
    }
}

void interpolateSnapshot(const EntitySnapshot &l_s1, const sf::Int32 &T1, const EntitySnapshot &l_s2, const sf::Int32 &T2, EntitySnapshot &l_target, const sf::Int32 &T_X)
{
    l_target.m_direction = l_s2.m_direction;
    l_target.m_health = l_s2.m_health;
    l_target.m_name = l_s2.m_name;
    l_target.m_state = l_s1.m_state;
    l_target.m_elevation = l_s1.m_elevation;

    l_target.m_position.x = interpolate<float>(T1, T2, l_s1.m_position.x, l_s2.m_position.x, T_X);
    l_target.m_position.y = interpolate<float>(T1, T2, l_s1.m_position.y, l_s2.m_position.y, T_X);

    l_target.m_velocity.x = interpolate<float>(T1, T2, l_s1.m_velocity.x, l_s2.m_velocity.x, T_X);
    l_target.m_velocity.y = interpolate<float>(T1, T2, l_s1.m_velocity.y, l_s2.m_velocity.y, T_X);

    l_target.m_acceleration.x = interpolate<float>(T1, T2, l_s1.m_acceleration.x, l_s2.m_acceleration.x, T_X);
    l_target.m_acceleration.y = interpolate<float>(T1, T2, l_s1.m_acceleration.y, l_s2.m_acceleration.y, T_X);
}

bool compareSnapshots(const EntitySnapshot &l_s1, const EntitySnapshot &l_s2, bool l_position, bool l_physics, bool l_state)
{
    if (l_position && (l_s1.m_position != l_s2.m_position || l_s1.m_elevation != l_s2.m_elevation)) {
        return false;
    }

    if (l_physics && (l_s1.m_velocity != l_s2.m_velocity
                      || l_s1.m_acceleration != l_s2.m_acceleration
                      || l_s1.m_direction != l_s2.m_direction))
    {
        return false;
    }

    if (l_state && (l_s1.m_state != l_s2.m_state)) {
        return false;
    }

    return true;
}
