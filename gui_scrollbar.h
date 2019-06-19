#pragma once

#include "gui_element.h"

enum class SliderType
{
    Horizontal,
    Vertical
};

class GUI_Scrollbar : public GUI_Element
{
public:
    GUI_Scrollbar(const std::string &l_name, GUI_Interface *l_owner);

    void setPosition(const sf::Vector2f &l_pos);
    void applyStyle();
    void updateStyle(const GUI_ElementState &l_state, const GUI_Style &l_style);
    void readIn(std::stringstream &l_stream);
    void onClick(const sf::Vector2f &l_mousePos);
    void onRelease();
    void onHover(const sf::Vector2f &l_mousePos);
    void onLeave();
    void update(float l_dt);
    void draw(sf::RenderTarget *l_target);

private:
    SliderType m_sliderType;
    sf::RectangleShape m_slider;
    sf::Vector2f m_moveMouseLast;
    int m_percentage;
};
