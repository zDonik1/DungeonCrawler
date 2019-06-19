#include "cliententitymanager.h"
#include "systemmanager.h"

ClientEntityManager::ClientEntityManager(SystemManager *l_sysMgr, TextureManager *l_textureMgr)
    : EntityManager(l_sysMgr)
    , m_textureManager(l_textureMgr)
{
    addComponentType<C_Position>(Component::Position);
    addComponentType<C_State>(Component::State);
    addComponentType<C_Movable>(Component::Movable);
    addComponentType<C_Controller>(Component::Controller);
    addComponentType<C_Collidable>(Component::Collidable);
    addComponentType<C_SpriteSheet>(Component::SpriteSheet);
    addComponentType<C_SoundEmitter>(Component::SoundEmitter);
    addComponentType<C_SoundListener>(Component::SoundListener);
    addComponentType<C_Client>(Component::Client);
    addComponentType<C_Health>(Component::Health);
    addComponentType<C_Name>(Component::Name);
    addComponentType<C_UI_Element>(Component::UI_Element);
}

ClientEntityManager::~ClientEntityManager()
{
}

int ClientEntityManager::addEntity(const std::string &l_entityFile, int l_id)
{
    int entityID = -1;

    std::ifstream file;
    file.open(Utils::getWorkingDirectory() + "media/Entities/" + l_entityFile + ".entity");
    if (!file.is_open()) {
        std::cout << "! Failed to load entity: " << l_entityFile << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') {
            continue;
        }

        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {

        }
        else if (type == "Attributes") {
            if (entityID != -1) {
                continue;
            }

            Bitset set = 0;
            Bitmask mask;
            keystream >> set;
            mask.setMask(set);
            entityID = EntityManager::addEntity(mask, l_id);
            if (entityID == -1) {
                return -1;
            }
        }
        else if (type == "Component") {
            if (entityID == -1) {
                continue;
            }

            unsigned int c_id = 0;
            keystream >> c_id;
            C_Base* component = getComponent<C_Base>(static_cast<EntityID>(entityID), static_cast<Component>(c_id));
            if (!component) {
                continue;
            }
            keystream >> *component;
            if (component->getType() == Component::SpriteSheet) {
                C_SpriteSheet* sheet = static_cast<C_SpriteSheet*>(component);
                sheet->create(m_textureManager);
            }
        }
    }

    file.close();
    return entityID;
}
