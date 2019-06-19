#include "soundmanager.h"
#include "statemanager.h"

SoundManager::SoundManager(AudioManager *l_audioMgr)
    : m_lastID(0)
    , m_numSounds(0)
    , m_elapsed(0.f)
    , m_audioManager(l_audioMgr)
{
}

SoundManager::~SoundManager()
{
    cleanUp();
}

void SoundManager::changeState(const StateType &l_state)
{
    pauseAll(m_currentState);
    unpauseAll(l_state);
    m_currentState = l_state;

    if (m_music.find(m_currentState) != m_music.end()) {
        return;
    }

    SoundInfo info("");
    sf::Music *music = nullptr;
    m_music.emplace(m_currentState, std::make_pair(info, music));
}

void SoundManager::removeState(const StateType &l_state)
{
    auto &stateSounds = m_audio.find(l_state)->second;
    for (auto &itr : stateSounds) {
        recycleSound(itr.first, itr.second.second, itr.second.first.m_name);
    }

    m_audio.erase(l_state);
    auto music = m_music.find(l_state);
    if (music == m_music.end()) {
        return;
    }

    if (music->second.second) {
        delete music->second.second;
        --m_numSounds;
    }

    m_music.erase(l_state);
}

void SoundManager::update(float l_dt)
{
    m_elapsed += l_dt;
    if (m_elapsed < 0.33f) {
        return;
    }

    m_elapsed = 0;
    auto &container = m_audio[m_currentState];
    for (auto itr = container.begin(); itr != container.end();) {
        if (!itr->second.second->getStatus()) {
            recycleSound(itr->first, itr->second.second, itr->second.first.m_name);
            itr = container.erase(itr);
            continue;
        }
        ++itr;
    }

    auto music = m_music.find(m_currentState);
    if (music == m_music.end()) {
        return;
    }

    if (!music->second.second) {
        return;
    }

    if (music->second.second->getStatus()) {
        return;
    }

    delete music->second.second;
    music->second.second = nullptr;
    --m_numSounds;
}

SoundID SoundManager::play(const std::string &l_sound, const sf::Vector3f &l_position, bool l_loop, bool l_relative)
{
    SoundProps *props = getSoundProperties(l_sound);
    if (!props) {
        return -1;
    }

    SoundID id;
    sf::Sound *sound = createSound(id, props->m_audioName);
    if (!sound) {
        return -1;
    }

    setUpSound(sound, props, l_loop, l_relative);
    sound->setPosition(l_position);
    SoundInfo info(props->m_audioName);
    m_audio[m_currentState].emplace(id, std::make_pair(info, sound));
    sound->play();
    return id;
}

bool SoundManager::play(const SoundID &l_id)
{
    auto &container = m_audio[m_currentState];
    auto sound = container.find(l_id);
    if (sound == container.end()) {
        return false;
    }

    sound->second.second->play();
    sound->second.first.m_manualPaused = false;
    return true;
}

bool SoundManager::stop(const SoundID &l_id)
{
    auto &container = m_audio[m_currentState];
    auto sound = container.find(l_id);
    if (sound == container.end()) {
        return false;
    }

    sound->second.second->stop();
    sound->second.first.m_manualPaused = true;
    return true;
}

bool SoundManager::pause(const SoundID &l_id)
{
    auto &container = m_audio[m_currentState];
    auto sound = container.find(l_id);
    if (sound == container.end()) {
        return false;
    }

    sound->second.second->pause();
    sound->second.first.m_manualPaused = true;
    return true;
}

bool SoundManager::playMusic(const std::string &l_musicID, float l_volume, bool l_loop)
{
    auto s = m_music.find(m_currentState);
    if (s == m_music.end()) {
        return false;
    }

    std::string path = m_audioManager->getPath(l_musicID);
    if (path == "") {
        return false;
    }

    if (!s->second.second) {
        s->second.second = new sf::Music();
        ++m_numSounds;
    }

    sf::Music *music = s->second.second;
    if (!music->openFromFile(Utils::getWorkingDirectory() + path)) {
        delete music;
        --m_numSounds;
        s->second.second = nullptr;
        std::cerr << "ERROR::SOUNDMANAGER::PLAYMUSIC::FAILED TO LOAD MUSIC FROM FILE " << l_musicID << std::endl;
        return false;
    }

    music->setLoop(l_loop);
    music->setVolume(l_volume);
    music->setRelativeToListener(true);
    music->play();
    s->second.first.m_name = l_musicID;
    return true;
}

bool SoundManager::playMusic(const StateType &l_state)
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end()) {
        return false;
    }

    if (!music->second.second) {
        return false;
    }

    music->second.second->play();
    music->second.first.m_manualPaused = false;
    return true;
}

bool SoundManager::stopMusic(const StateType &l_state)
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end()) {
        return false;
    }

    if (!music->second.second) {
        return false;
    }

    music->second.second->stop();
    delete music->second.second;
    music->second.second = nullptr;
    --m_numSounds;
    return true;
}

bool SoundManager::pauseMusic(const StateType &l_state)
{
    auto music = m_music.find(m_currentState);
    if (music == m_music.end()) {
        return false;
    }

    if (!music->second.second) {
        return false;
    }

    music->second.second->pause();
    music->second.first.m_manualPaused = true;
    return true;
}

bool SoundManager::isPlaying(const SoundID &l_id)
{
    auto &container = m_audio[m_currentState];
    auto sound = container.find(l_id);
    return (sound != container.end() ? sound->second.second->getStatus() : false);
}

SoundProps *SoundManager::getSoundProperties(const std::string &l_soundName)
{
    auto properties = m_properties.find(l_soundName);
    if (properties == m_properties.end()) {
        if (!loadProperties(l_soundName)) {
            return nullptr;
        }

        properties = m_properties.find(l_soundName);
    }

    return &properties->second;
}

bool SoundManager::setPosition(const SoundID &l_id, const sf::Vector3f &l_pos)
{
    auto &container = m_audio[m_currentState];
    auto sound = container.find(l_id);
    if (sound == container.end()) {
        return false;
    }

    sound->second.second->setPosition(l_pos);
    return true;
}

bool SoundManager::loadProperties(const std::string &l_name)
{
    std::ifstream file;
    file.open(Utils::getWorkingDirectory() + "media/Sounds/" + l_name + ".sound");
    if (!file.is_open()) {
        std::cerr << "ERROR::SOUNDMANAGER::LOADPROPERTIES::FAILED TO LOAD SOUND " << l_name << std::endl;
        return false;
    }

    SoundProps props("");
    std::string line;
    while(std::getline(file, line)) {
        if (line[0] == '|') {
            continue;
        }

        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Audio") {
            keystream >> props.m_audioName;
        }
        else if (type == "Volume") {
            keystream >> props.m_volume;
        }
        else if (type == "Pitch") {
            keystream >> props.m_pitch;
        }
        else if (type == "Distance") {
            keystream >> props.m_minDistance;
        }
        else if (type == "Attenuation") {
            keystream >> props.m_attenuation;
        }
    }

    file.close();
    if (props.m_audioName == "") {
        return false;
    }

    m_properties.emplace(l_name, props);
    return true;
}

void SoundManager::pauseAll(const StateType &l_state)
{
    auto &container = m_audio[l_state];
    for (auto itr = container.begin(); itr != container.end();) {
        if (!itr->second.second->getStatus()) {
            recycleSound(itr->first, itr->second.second, itr->second.first.m_name);
            itr = container.erase(itr);
            continue;
        }

        itr->second.second->pause();
        ++itr;
    }

    auto music = m_music.find(l_state);
    if (music == m_music.end()) {
        return;
    }

    if (!music->second.second) {
        return;
    }

    music->second.second->pause();
}

void SoundManager::unpauseAll(const StateType &l_state)
{
    auto &container = m_audio[l_state];
    for (auto &itr : container) {
        if (itr.second.first.m_manualPaused) {
            continue;
        }

        itr.second.second->play();
    }

    auto music = m_music.find(l_state);
    if (music == m_music.end()) {
        return;
    }

    if (!music->second.second || music->second.first.m_manualPaused) {
        return;
    }
    music->second.second->play();
}

sf::Sound *SoundManager::createSound(SoundID &l_id, const std::string &l_audioName)
{
    sf::Sound *sound = nullptr;
    if (!m_recycled.empty() && (m_numSounds >= Max_Sounds || m_recycled.size() >= Sound_Cache)) {
        auto itr = m_recycled.begin();
        while (itr != m_recycled.end()) {
            if (itr->first.second == l_audioName) {
                break;
            }

            ++itr;
        }

        if (itr == m_recycled.end()) {
            auto element = m_recycled.begin();
            l_id = element->first.first;
            m_audioManager->releaseResource(element->first.second);
            m_audioManager->requireResource(l_audioName);
            sound = element->second;
            sound->setBuffer(*m_audioManager->getResource(l_audioName));
            m_recycled.erase(element);
        }
        else {
            l_id = itr->first.first;
            sound = itr->second;
            m_recycled.erase(itr);
        }

        return sound;
    }

    if (m_numSounds < Max_Sounds) {
        if (m_audioManager->requireResource(l_audioName)) {
            sound = new sf::Sound();
            l_id = m_lastID;
            ++m_lastID;
            ++m_numSounds;
            sound->setBuffer(*m_audioManager->getResource(l_audioName));
            return sound;
        }
    }

    std::cerr << "ERROR::SOUNDMANAGER::CREATESOUND::FAILED TO CREATE SOUND!" << std::endl;
    return nullptr;
}

void SoundManager::setUpSound(sf::Sound *l_snd, const SoundProps *l_props, bool l_loop, bool l_relative)
{
    l_snd->setVolume(l_props->m_volume);
    l_snd->setPitch(l_props->m_pitch);
    l_snd->setMinDistance(l_props->m_minDistance);
    l_snd->setAttenuation(l_props->m_attenuation);
    l_snd->setLoop(l_loop);
    l_snd->setRelativeToListener(l_relative);
}

void SoundManager::recycleSound(const SoundID &l_id, sf::Sound *l_snd, const std::string &l_name)
{
    m_recycled.emplace_back(std::make_pair(l_id, l_name), l_snd);
}

void SoundManager::cleanUp()
{
    for (auto &state : m_audio) {
        for (auto &sound : state.second) {
            m_audioManager->releaseResource(sound.second.first.m_name);
            delete sound.second.second;
        }
    }
    m_audio.clear();

    for (auto &recycled : m_recycled) {
        m_audioManager->releaseResource(recycled.first.second);
        delete recycled.second;
    }
    m_recycled.clear();

    for (auto &music : m_music) {
        if (music.second.second) {
            delete music.second.second;
        }
    }
    m_music.clear();

    m_properties.clear();
    m_numSounds = 0;
    m_lastID = 0;
}


