#include "clientsystemmanager.h"

ClientSystemManager::ClientSystemManager(TextureManager *l_textureMgr, FontManager *l_fontMgr)
    : m_textureMgr(l_textureMgr)
    , m_fontMgr(l_fontMgr)
{
    addSystem<S_State>(System::State);
    addSystem<S_Control>(System::Control);
    addSystem<S_Movement>(System::Movement);
    addSystem<S_Collision>(System::Collision);
    addSystem<S_SheetAnimation>(System::SheetAnimation);
    addSystem<S_Network>(System::Network);
    addSystem<S_Sound>(System::Sound);
    addSystem<S_Renderer>(System::Renderer);
    addSystem<S_CharacterUI>(System::Character_UI);
}

ClientSystemManager::~ClientSystemManager()
{
}

TextureManager *ClientSystemManager::getTextureManager()
{
    return m_textureMgr;
}

FontManager *ClientSystemManager::getFontManager()
{
    return m_fontMgr;
}

void ClientSystemManager::draw(Window *l_wind, unsigned int l_elevation)
{
    auto itr = m_systems.find(System::Renderer);
    if (itr != m_systems.end()) {
        S_Renderer *system = (S_Renderer*)itr->second;
        system->render(l_wind, l_elevation);
    }
    itr = m_systems.find(System::Character_UI);
    if (itr != m_systems.end()) {
        S_CharacterUI *ui = (S_CharacterUI*)itr->second;
        ui->render(l_wind);
    }
}
