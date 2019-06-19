#include "s_renderer.h"
#include "systemmanager.h"
#include "c_spritesheet.h"

S_Renderer::S_Renderer(SystemManager *l_systemMgr)
    : S_Base(System::Renderer, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::SpriteSheet);
    m_requiredComponents.push_back(req);
    req.clear();

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Direction_Changed, this);
}

S_Renderer::~S_Renderer()
{

}

void S_Renderer::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Position *position = entities->getComponent<C_Position>(entity, Component::Position);
        C_Drawable *drawable = nullptr;
        if (entities->hasComponent(entity, Component::SpriteSheet)) {
            drawable = entities->getComponent<C_Drawable>(entity, Component::SpriteSheet);
        }
        else {
            continue;
        }

        drawable->updatePosition(position->getPosition());
    }
}

void S_Renderer::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
    if (l_event == EntityEvent::Moving_Left ||
            l_event == EntityEvent::Moving_Right ||
            l_event == EntityEvent::Moving_Up ||
            l_event == EntityEvent::Moving_Down ||
            l_event == EntityEvent::Elevation_Change ||
            l_event == EntityEvent::Spawned)
    {
        sortDrawables();
    }
}

void S_Renderer::notify(const Message &l_message)
{
    if (hasEntity(l_message.m_receiver)) {
        EntityMessage m = (EntityMessage)l_message.m_type;
        switch (m) {
        case EntityMessage::Direction_Changed:
            setSheetDirection(l_message.m_receiver, (Direction)l_message.m_int);

            break;
        }
    }
}

void S_Renderer::render(Window *l_wind, unsigned int l_layer)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Position *position = entities->getComponent<C_Position>(entity, Component::Position);
        if (position->getElevation() < l_layer) {
            continue;
        }
        if (position->getElevation() > l_layer) {
            break;
        }

        C_Drawable *drawable = nullptr;
        if (!entities->hasComponent(entity, Component::SpriteSheet)) {
            continue;
        }

        drawable = entities->getComponent<C_Drawable>(entity, Component::SpriteSheet);
        sf::FloatRect drawableBounds;
        drawableBounds.left = position->getPosition().x - (drawable->getSize().x / 2);
        drawableBounds.top = position->getPosition().y - (drawable->getSize().y / 2);
        drawableBounds.width = drawable->getSize().x;
        drawableBounds.height = drawable->getSize().y;
        if (!l_wind->getViewSpace().intersects(drawableBounds)) {
            continue;
        }

        drawable->draw(l_wind->getRenderWindow());
    }
}

void S_Renderer::setSheetDirection(const EntityID &l_entity, const Direction &l_dir)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    if (!entities->hasComponent(l_entity, Component::SpriteSheet)) {
        return;
    }

    C_SpriteSheet *sheet = entities->getComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    sheet->getSpriteSheet()->setDirection(l_dir);
}

void S_Renderer::sortDrawables()
{
    EntityManager *e_mgr = m_systemManager->getEntityManager();
    std::sort(m_entities.begin(), m_entities.end(), [e_mgr] (unsigned int l_1, unsigned int l_2)
    {
        auto pos1 = e_mgr->getComponent<C_Position>(l_1, Component::Position);
        auto pos2 = e_mgr->getComponent<C_Position>(l_2, Component::Position);
        if (pos1->getElevation() == pos2->getElevation()) {
            return pos1->getPosition().y < pos2->getPosition().y;
        }

        return pos1->getElevation() < pos2->getElevation();
    });
}
