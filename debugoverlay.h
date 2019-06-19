#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class DebugOverlay
{
public:
    DebugOverlay()
    {
        m_debug = false;
    }

    ~DebugOverlay()
    {
        while (m_drawables.begin() != m_drawables.end()) {
            delete m_drawables.back();
            m_drawables.pop_back();
        }
    }

    bool getDebug()
    {
        return m_debug;
    }

    void setDebug(bool l_debug)
    {
        m_debug = l_debug;
    }

    void add(sf::Drawable *l_drawable)
    {
        m_drawables.push_back(l_drawable);
    }

    void draw(sf::RenderWindow *l_wind)
    {
        if (m_debug) {
            while (m_drawables.begin() != m_drawables.end()) {
                l_wind->draw(*m_drawables.back());
                delete m_drawables.back();
                m_drawables.pop_back();
            }
        }
    }

private:
    std::vector<sf::Drawable*> m_drawables;
    bool m_debug;
};
