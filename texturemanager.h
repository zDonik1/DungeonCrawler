#pragma once

#include "resourcemanager.h"
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture>
{
public:
    TextureManager() : ResourceManager("textures.cfg") {}

    sf::Texture *load(const std::string& l_path)
    {
        sf::Texture *texture = new sf::Texture();
        if (!texture->loadFromFile(Utils::getWorkingDirectory() + l_path)) {
            delete texture;
            texture = nullptr;

            std::cerr << "ERROR::TEXTUREMANAGER::LOAD::FAILED TO LOAD TEXTURE:  " << l_path << std::endl;
        }

        return texture;
    }
};
