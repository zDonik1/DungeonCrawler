#pragma once

#include "resourcemanager.h"
#include <SFML/Graphics/Font.hpp>

class FontManager : public ResourceManager<FontManager, sf::Font>
{
public:
    FontManager() : ResourceManager("fonts.cfg")
    {
    }

    sf::Font *load(const std::string &l_path)
    {
        sf::Font *font = new sf::Font();
        if (!font->loadFromFile(Utils::getWorkingDirectory() + l_path)) {
            delete font;
            font = nullptr;
            std::cerr << "ERROR::FONTMANAGER::LOAD::FAILED TO LOAD FONT: " << l_path << std::endl;
        }

        return font;
    }
};
