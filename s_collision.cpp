#include "s_collision.h"
#include "systemmanager.h"
#include "map.h"

S_Collision::S_Collision(SystemManager *l_systemMgr)
    : S_Base(System::Collision, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::Collidable);
    m_requiredComponents.push_back(req);
    req.clear();

    m_gameMap = nullptr;
}

S_Collision::~S_Collision()
{
}

void S_Collision::setMap(Map *l_map)
{
    m_gameMap = l_map;
}

void S_Collision::update(float l_dt)
{
    if (!m_gameMap) {
        return;
    }

    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Position *position = entities->getComponent<C_Position>(entity, Component::Position);
        C_Collidable *collidable = entities->getComponent<C_Collidable>(entity, Component::Collidable);

        collidable->setPosition(position->getPosition());
        collidable->resetCollisionFlags();
        checkOutOfBounds(position, collidable);
        mapCollisions(entity, position, collidable);
    }

    entityCollisions();
}

void S_Collision::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{

}

void S_Collision::notify(const Message &l_message)
{

}

void S_Collision::checkOutOfBounds(C_Position *l_pos, C_Collidable *l_col)
{
    unsigned int tileSize = m_gameMap->getTileSize();

    if (l_pos->getPosition().x < 0) {
        l_pos->setPosition(0.f, l_pos->getPosition().y);
        l_col->setPosition(l_pos->getPosition());
    }
    else if (l_pos->getPosition().x > m_gameMap->getMapSize().x * tileSize) {
        l_pos->setPosition(m_gameMap->getMapSize().x * tileSize, l_pos->getPosition().y);
        l_col->setPosition(l_pos->getPosition());
    }

    if (l_pos->getPosition().y < 0) {
        l_pos->setPosition(l_pos->getPosition().x, 0.f);
        l_col->setPosition(l_pos->getPosition());
    }
    else if (l_pos->getPosition().y > m_gameMap->getMapSize().y * tileSize) {
        l_pos->setPosition(l_pos->getPosition().x, m_gameMap->getMapSize().y * tileSize);
        l_col->setPosition(l_pos->getPosition());
    }
}

void S_Collision::mapCollisions(const EntityID &l_entity, C_Position *l_pos, C_Collidable *l_col)
{
    unsigned int tileSize = m_gameMap->getTileSize();
    Collisions c;

    sf::FloatRect entityAABB = l_col->getCollidable();
    int fromX = floor(entityAABB.left / tileSize);
    int toX = floor((entityAABB.left + entityAABB.width) / tileSize);
    int fromY = floor(entityAABB.top / tileSize);
    int toY = floor((entityAABB.top + entityAABB.height) / tileSize);

    for (int x = fromX; x <= toX; ++x) {
        for (int y = fromY; y <= toY; ++y) {
            for (int l = 0; l < Sheet::Num_Layers; ++l) {
                Tile *t = m_gameMap->getTile(x, y, l);
                if (!t) {
                    continue;
                }
                if (!t->m_solid) {
                    continue;
                }

                sf::FloatRect tileAABB(x * tileSize, y * tileSize, tileSize, tileSize);
                sf::FloatRect intersection;
                entityAABB.intersects(tileAABB, intersection);
                float s = intersection.width * intersection.height;

                c.emplace_back(s, t->m_properties, tileAABB);
                break;
            }
        }
    }

    if (c.empty()) {
        return;
    }

    std::sort(c.begin(), c.end(), [] (CollisionElement &l_1, CollisionElement &l_2)
    {
        return l_1.m_area > l_2.m_area;
    });

    for (auto &col : c) {
        entityAABB = l_col->getCollidable();
        if (!entityAABB.intersects(col.m_tileBounds)) {
            continue;
        }

        float xDiff = (entityAABB.left + (entityAABB.width / 2)) - (col.m_tileBounds.left + (col.m_tileBounds.width / 2));
        float yDiff = (entityAABB.top + (entityAABB.height / 2)) - (col.m_tileBounds.top + (col.m_tileBounds.height / 2));
        float resolve = 0;
        if (std::abs(xDiff) > std::abs(yDiff)) {
            if (xDiff > 0) {
                resolve = (col.m_tileBounds.left + tileSize) - entityAABB.left;
            }
            else {
                resolve = -((entityAABB.left + entityAABB.width) - col.m_tileBounds.left);
            }

            l_pos->moveBy(resolve, 0);
            l_col->setPosition(l_pos->getPosition());
            m_systemManager->addEvent(l_entity, (EventID)EntityEvent::Colliding_X);
            l_col->collideOnX();
        }
        else {
            if (yDiff > 0) {
                resolve = (col.m_tileBounds.top + tileSize) - entityAABB.top;
            }
            else {
                resolve = -((entityAABB.top + entityAABB.height) - col.m_tileBounds.top);
            }

            l_pos->moveBy(0, resolve);
            l_col->setPosition(l_pos->getPosition());
            m_systemManager->addEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
            l_col->collideOnY();
        }
    }
}

void S_Collision::entityCollisions()
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr) {
        for (auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr2) {
            C_Collidable *collidable1 = entities->getComponent<C_Collidable>(*itr, Component::Collidable);
            C_Collidable *collidable2 = entities->getComponent<C_Collidable>(*itr2, Component::Collidable);
            if (collidable1->getCollidable().intersects(collidable2->getCollidable())) {
                // Entity-on-entity collision.
            }
        }
    }
}
