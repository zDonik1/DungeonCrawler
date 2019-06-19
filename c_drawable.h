#pragma once

#include <SFML/Graphics.hpp>
#include "c_base.h"

class C_Drawable : public C_Base
{
public:
    C_Drawable(const Component &l_type)
        : C_Base(l_type)
    {
    }

    virtual ~C_Drawable()
    {
    }

    virtual void updatePosition(const sf::Vector2f &l_vec) = 0;
    virtual const sf::Vector2u &getSize() = 0;
    virtual void draw(sf::RenderWindow *l_wind) = 0;
};
