#pragma once

#include "c_drawable.h"
#include "spritesheet.h"

class C_SpriteSheet : public C_Drawable
{
public:
    C_SpriteSheet()
        : C_Drawable(Component::SpriteSheet)
        , m_spriteSheet(nullptr)
    {
    }

    ~C_SpriteSheet()
    {
        if (m_spriteSheet) {
            delete m_spriteSheet;
        }
    }

    void readIn(std::stringstream &l_stream)
    {
        l_stream >> m_sheetName;
    }

    void create(TextureManager *l_textureMgr, const std::string &l_name = "")
    {
        if (m_spriteSheet) {
            return;
        }

        m_spriteSheet = new SpriteSheet(l_textureMgr);
        m_spriteSheet->loadSheet("media/SpriteSheets/" + (l_name != "" ? l_name : m_sheetName) + ".sheet");
    }

    SpriteSheet *getSpriteSheet()
    {
        return m_spriteSheet;
    }

    void updatePosition(const sf::Vector2f &l_vec)
    {
        m_spriteSheet->setSpritePosition(l_vec);
    }

    const sf::Vector2u &getSize()
    {
        return sf::Vector2u(m_spriteSheet->getSpriteSize().x, m_spriteSheet->getSpriteSize().y);
    }

    void draw(sf::RenderWindow *l_wind)
    {
        if (!m_spriteSheet) {
            return;
        }

        m_spriteSheet->draw(l_wind);
    }

private:
    SpriteSheet *m_spriteSheet;
    std::string m_sheetName;
};
