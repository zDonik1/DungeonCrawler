#include "s_characterui.h"
#include "clientsystemmanager.h"

S_CharacterUI::S_CharacterUI(SystemManager *l_systemMgr)
    : S_Base(System::Character_UI, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::UI_Element);
    req.turnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);

    req.clearBit((unsigned int)Component::Health);
    req.turnOnBit((unsigned int)Component::Name);
    m_requiredComponents.push_back(req);

    ClientSystemManager *mgr = (ClientSystemManager*)m_systemManager;
    mgr->getTextureManager()->requireResource("HeartBar");
    mgr->getFontManager()->requireResource("Main");
    sf::Texture *txtr = mgr->getTextureManager()->getResource("HeartBar");
    txtr->setRepeated(true);
    m_heartBarSize = txtr->getSize();
    m_heartBar.setTexture(*txtr);
    m_heartBar.setScale(0.5f, 0.5f);
    m_heartBar.setOrigin(m_heartBarSize.x / 2, m_heartBarSize.y);

    m_nickname.setFont(*mgr->getFontManager()->getResource("Main"));
    m_nickname.setCharacterSize(9);
    m_nickname.setColor(sf::Color::White);
    m_nickbg.setFillColor(sf::Color(100, 100, 100, 100));
}

S_CharacterUI::~S_CharacterUI()
{
    ClientSystemManager *mgr = (ClientSystemManager*)m_systemManager;
    mgr->getTextureManager()->releaseResource("HeartBar");
    mgr->getFontManager()->releaseResource("Main");
}

void S_CharacterUI::update(float l_dt)
{
}

void S_CharacterUI::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_CharacterUI::notify(const Message &l_message)
{
}

void S_CharacterUI::render(Window *l_wind)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Health *health = entities->getComponent<C_Health>(entity, Component::Health);
        C_Name *name = entities->getComponent<C_Name>(entity, Component::Name);
        C_Position *pos = entities->getComponent<C_Position>(entity, Component::Position);
        C_UI_Element *ui = entities->getComponent<C_UI_Element>(entity, Component::UI_Element);

        if (health) {
            m_heartBar.setTextureRect(sf::IntRect(0, 0, m_heartBarSize.x * health->getHeatlh(), m_heartBarSize.y));
            m_heartBar.setOrigin(m_heartBarSize.x * health->getHeatlh() / 2, m_heartBarSize.y);
            m_heartBar.setPosition(pos->getPosition() + ui->getOffset());
            l_wind->getRenderWindow()->draw(m_heartBar);
        }
        if (name) {
            m_nickname.setString("you fking rat"); /// TO DO
            m_nickname.setOrigin(m_nickname.getLocalBounds().width / 2, m_nickname.getLocalBounds().height / 2);
            if (health) {
                m_nickname.setPosition(m_heartBar.getPosition().x, m_heartBar.getPosition().y - m_heartBarSize.y);
            }
            else {
                m_nickname.setPosition(pos->getPosition() + ui->getOffset());
            }
            m_nickbg.setSize(sf::Vector2f(m_nickname.getGlobalBounds().width + 2, m_nickname.getCharacterSize() + 1));
            m_nickbg.setOrigin(m_nickbg.getSize().x / 2, m_nickbg.getSize().y / 2);
            m_nickbg.setPosition(m_nickname.getPosition().x + 1, m_nickname.getPosition().y + 1);
            l_wind->getRenderWindow()->draw(m_nickbg);
            l_wind->getRenderWindow()->draw(m_nickname);
        }
    }
}

