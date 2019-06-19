#include "gui_label.h"

GUI_Label::GUI_Label(const std::string &l_name, GUI_Interface *l_owner)
    : GUI_Element(l_name, GUI_ElementType::Label, l_owner)
{
}

void GUI_Label::readIn(std::stringstream &l_stream)
{
    std::string content;
    Utils::readQuotedString(l_stream, content);
    m_visual.m_text.setString(content);
}

void GUI_Label::onClick(const sf::Vector2f &l_mousePos)
{
    setState(GUI_ElementState::Clicked);
}

void GUI_Label::onRelease()
{
    setState(GUI_ElementState::Neutral);
}

void GUI_Label::onHover(const sf::Vector2f &l_mousePos)
{
    setState(GUI_ElementState::Focused);
}

void GUI_Label::onLeave()
{
    setState(GUI_ElementState::Neutral);
}

void GUI_Label::update(float l_dt)
{

}

void GUI_Label::draw(sf::RenderTarget *l_target)
{
    l_target->draw(m_visual.m_backgroundSolid);
    if (m_style[m_state].m_glyph != "") {
        l_target->draw(m_visual.m_glyph);
    }
    l_target->draw(m_visual.m_text);
}


