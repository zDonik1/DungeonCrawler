#include "gui_element.h"
#include "gui_interface.h"
#include "gui_manager.h"
#include "sharedcontext.h"

GUI_Element::GUI_Element(const std::string &l_name, const GUI_ElementType &l_type, GUI_Interface *l_owner)
    : m_name(l_name)
    , m_type(l_type)
    , m_state(GUI_ElementState::Neutral)
    , m_owner(l_owner)
    , m_needsRedraw(false)
    , m_active(true)
    , m_isControl(false)
{
}

GUI_Element::~GUI_Element()
{
    releaseResources();
}

void GUI_Element::updateStyle(const GUI_ElementState &l_state, const GUI_Style &l_style)
{
    if (l_style.m_backgroundImage != m_style[l_state].m_backgroundImage) {
        releaseTexture(m_style[l_state].m_backgroundImage);
        requireTexture(l_style.m_backgroundImage);
    }

    if (l_style.m_glyph != m_style[l_state].m_glyph) {
        releaseTexture(m_style[l_state].m_glyph);
        requireTexture(l_style.m_glyph);
    }

    if (l_style.m_textFont != m_style[l_state].m_textFont) {
        releaseFont(m_style[l_state].m_textFont);
        requireFont(l_style.m_textFont);
    }

    m_style[l_state] = l_style;
    if (l_state == m_state) {
        setRedraw(true);
        applyStyle();
    }
}

void GUI_Element::applyStyle()
{
    applyTextStyle();
    applyBgStyle();
    applyGlyphStyle();
    if (m_owner != this && !isControl()) {
        m_owner->adjustContentSize(this);
    }
}

bool GUI_Element::isInside(const sf::Vector2f &l_point) const
{
    sf::Vector2f position = getGlobalPosition();

    return (l_point.x >= position.x &&
            l_point.y >= position.y &&
            l_point.x <= position.x + m_style.at(m_state).m_size.x &&
            l_point.y <= position.y + m_style.at(m_state).m_size.y);
}

bool GUI_Element::isControl() const
{
    return m_isControl;
}

bool GUI_Element::isActive() const
{
    return m_active;
}

bool GUI_Element::needsRedraw() const
{
    return m_needsRedraw;
}

sf::Vector2f GUI_Element::getPosition() const
{
    return m_position;
}

sf::Vector2f GUI_Element::getGlobalPosition() const
{
    sf::Vector2f position = getPosition();
    if (m_owner == nullptr || m_owner == this) {
        return position;
    }

    position += m_owner->getGlobalPosition();
    if (isControl()) {
        return position;
    }

    position.x -= m_owner->m_scrollHorizontal;
    position.y -= m_owner->m_scrollVertical;
    return position;
}

const sf::Vector2f &GUI_Element::getSize() const
{
    return m_style.at(m_state).m_size;
}

std::string GUI_Element::getText() const
{
    return m_visual.m_text.getString();
}

GUI_ElementState GUI_Element::getState() const
{
    return m_state;
}

GUI_ElementType GUI_Element::getType() const
{
    return m_type;
}

void GUI_Element::setText(const std::string &l_text)
{
    m_visual.m_text.setString(l_text);
    setRedraw(true);
}

void GUI_Element::setActive(const bool &l_active)
{
    if (l_active != m_active) {
        m_active = l_active;
        setRedraw(true);
    }
}

void GUI_Element::setRedraw(bool l_redraw)
{
    m_needsRedraw = l_redraw;
}

void GUI_Element::setState(const GUI_ElementState &l_state)
{
    if (m_state == l_state) {
        return;
    }

    m_state = l_state;
    setRedraw(true);
}

void GUI_Element::setPosition(const sf::Vector2f &l_pos)
{
    m_position = l_pos;
    if (m_owner == nullptr || m_owner == this) {
        return;
    }

    const auto &padding = m_owner->getPadding();
    if (m_position.x < padding.x) {
        m_position.x = padding.x;
    }
    if (m_position.y < padding.y) {
        m_position.y = padding.y;
    }
}

void GUI_Element::setName(const std::string &l_name)
{
    m_name = l_name;
}

void GUI_Element::setOwner(GUI_Interface *l_owner)
{
    m_owner = l_owner;
}

void GUI_Element::applyTextStyle()
{
    FontManager *fonts = m_owner->getManager()->getContext()->m_fontManager;
    const GUI_Style &currentStyle = m_style[m_state];
    if (currentStyle.m_textFont != "") {
        m_visual.m_text.setFont(*fonts->getResource(currentStyle.m_textFont));
        m_visual.m_text.setColor(currentStyle.m_textColor);
        m_visual.m_text.setCharacterSize(currentStyle.m_textSize);
        if (currentStyle.m_textCenterOrigin) {
            sf::FloatRect rect = m_visual.m_text.getLocalBounds();
            m_visual.m_text.setOrigin(rect.left + rect.width / 2.f, rect.top + rect.height / 2.f);
        }
        else {
            m_visual.m_text.setOrigin(0.f, 0.f);
        }
    }
    m_visual.m_text.setPosition(m_position + currentStyle.m_textPadding);
}

void GUI_Element::applyBgStyle()
{
    TextureManager *textures = m_owner->getManager()->getContext()->m_textureManager;
    const GUI_Style &currentStyle = m_style[m_state];
    if (currentStyle.m_backgroundImage != "") {
        m_visual.m_backgroundImage.setTexture(*textures->getResource(currentStyle.m_backgroundImage));
        m_visual.m_backgroundImage.setColor(currentStyle.m_backgroundImageColor);
    }
    m_visual.m_backgroundImage.setPosition(m_position);
    m_visual.m_backgroundSolid.setSize(sf::Vector2f(currentStyle.m_size));
    m_visual.m_backgroundSolid.setFillColor(currentStyle.m_backgroundColor);
    m_visual.m_backgroundSolid.setPosition(m_position);
}

void GUI_Element::applyGlyphStyle()
{
    TextureManager *textures = m_owner->getManager()->getContext()->m_textureManager;
    const GUI_Style &currentStyle = m_style[m_state];
    if (currentStyle.m_glyph != "") {
        m_visual.m_glyph.setTexture(*textures->getResource(currentStyle.m_glyph));
    }
    m_visual.m_glyph.setPosition(m_position + currentStyle.m_glyphPadding);
}

void GUI_Element::requireTexture(const std::string &l_name)
{
    if (l_name == "") {
        return;
    }

    m_owner->getManager()->getContext()->m_textureManager->requireResource(l_name);
}

void GUI_Element::requireFont(const std::string &l_name)
{
    if (l_name == "") {
        return;
    }

    m_owner->getManager()->getContext()->m_fontManager->requireResource(l_name);
}

void GUI_Element::releaseTexture(const std::string &l_name)
{
    if (l_name == "") {
        return;
    }

    m_owner->getManager()->getContext()->m_textureManager->releaseResource(l_name);
}

void GUI_Element::releaseFont(const std::string &l_name)
{
    if (l_name == "") {
        return;
    }

    m_owner->getManager()->getContext()->m_fontManager->releaseResource(l_name);
}

void GUI_Element::releaseResources()
{
    for (auto &itr : m_style) {
        releaseTexture(itr.second.m_backgroundImage);
        releaseTexture(itr.second.m_glyph);
        releaseFont(itr.second.m_textFont);
    }
}

