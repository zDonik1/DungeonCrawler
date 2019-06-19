#pragma once

#include "s_base.h"
#include "entitysnapshot.h"
#include "cliententitymanager.h"
#include "entitymessages.h"
#include "client.h"
#include "s_movement.h"
#include "s_state.h"
#include "netsettings.h"

template<class T>
inline T interpolate(const sf::Int32 &T1, const sf::Int32 &T2, const T &T1_val, const T &T2_val, const sf::Int32 &T_X)
{
    return (((T2_val - T1_val) / (T2 - T1)) * (T_X - T1)) + T1_val;
}

void interpolateSnapshot(const EntitySnapshot &l_s1, const sf::Int32 &T1,
                         const EntitySnapshot &l_s2, const sf::Int32 &T2,
                         EntitySnapshot &l_target, const sf::Int32 &T_X);

bool compareSnapshots(const EntitySnapshot &l_s1, const EntitySnapshot &l_s2, bool l_position = true, bool l_physics = true, bool l_state = true);

using SnapshotMap = std::unordered_map<EntityID, EntitySnapshot>;

struct SnapshotDetails
{
    SnapshotMap m_snapshots;
};

using SnapshotContainer = std::map<sf::Int32, SnapshotDetails>;
using OutgoingMessages = std::unordered_map<EntityMessage, std::vector<Message>>;

class S_Network : public S_Base
{
public:
    S_Network(SystemManager *l_systemMgr);
    ~S_Network();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    void setClient(Client *l_client);
    void setPlayerID(const EntityID &l_entity);

    void addSnapshot(const EntityID& l_entity, const sf::Int32 &l_timestamp, EntitySnapshot &l_snapshot);
    void sendPlayerOutgoing();
    void clearSnapshots();

private:
    void applyEntitySnapshot(const EntityID &l_entity, const EntitySnapshot &l_snapshot, bool l_applyPhysics);
    void performInterpolation();

    SnapshotContainer m_entitySnapshots;
    EntityID m_player;
    OutgoingMessages m_outgoing;
    Client *m_client;
    sf::Time m_playerUpdateTimer;
};

