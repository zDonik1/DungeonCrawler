#pragma once

#include "resourcemanager.h"
#include <SFML/Audio/SoundBuffer.hpp>

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>
{
public:
    AudioManager() : ResourceManager("audio.cfg")
    {
    }

    sf::SoundBuffer *load(const std::string &l_path)
    {
        sf::SoundBuffer *sound = new sf::SoundBuffer();
        if (!sound->loadFromFile(Utils::getWorkingDirectory() + l_path)) {
            delete sound;
            sound = nullptr;
            std::cerr << "AUDIOMANAGER::LOAD::FAILED TO LOAD SOUND " << l_path << std::endl;
        }

        return sound;
    }
};
