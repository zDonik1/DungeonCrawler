#pragma once

#include "gui_element.h"
#include "utilities.h"

class GUI_Label : public GUI_Element
{
public:
    GUI_Label(const std::string &l_name, GUI_Interface *l_owner);

    void readIn(std::stringstream &l_stream);
    void onClick(const sf::Vector2f &l_mousePos);
    void onRelease();
    void onHover(const sf::Vector2f &l_mousePos);
    void onLeave();
    void update(float l_dt);
    void draw(sf::RenderTarget *l_target);
};
