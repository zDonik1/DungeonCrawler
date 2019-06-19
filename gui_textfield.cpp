#include "gui_textfield.h"

GUI_Textfield::GUI_Textfield(const std::string &l_name, GUI_Interface *l_owner)
    : GUI_Element(l_name, GUI_ElementType::Textfield, l_owner)
{
}

void GUI_Textfield::readIn(std::stringstream &l_stream)
{
    std::string content;
    Utils::readQuotedString(l_stream, content);
    m_visual.m_text.setString(content);
}

void GUI_Textfield::onClick(const sf::Vector2f &l_mousePos)
{
    setState(GUI_ElementState::Clicked);
}

void GUI_Textfield::onRelease()
{
}

void GUI_Textfield::onHover(const sf::Vector2f &l_mousePos)
{
    setState(GUI_ElementState::Focused);
}

void GUI_Textfield::onLeave()
{
    setState(GUI_ElementState::Neutral);
}

void GUI_Textfield::update(float l_dt)
{

}

void GUI_Textfield::draw(sf::RenderTarget *l_target)
{
    l_target->draw(m_visual.m_backgroundSolid);
    if (m_style[m_state].m_glyph != "") {
        l_target->draw(m_visual.m_glyph);
    }
    l_target->draw(m_visual.m_text);
}
