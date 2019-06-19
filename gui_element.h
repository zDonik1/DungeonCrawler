#pragma once

#include <unordered_map>
#include "gui_style.h"

enum class GUI_ElementType
{
    None,
    Window,
    Label,
    Button,
    Scrollbar,
    Textfield
};

using ElementStyles = std::unordered_map<GUI_ElementState, GUI_Style>;

class GUI_Interface;

class GUI_Element
{
    friend class GUI_Interface;
public:
    GUI_Element(const std::string &l_name, const GUI_ElementType &l_type, GUI_Interface *l_owner);
    virtual ~GUI_Element();

    virtual void readIn(std::stringstream &l_stream) = 0;
    virtual void onClick(const sf::Vector2f &l_mousePos) = 0;
    virtual void onRelease() = 0;
    virtual void onHover(const sf::Vector2f &l_mousePos) = 0;
    virtual void onLeave() = 0;
    virtual void update(float l_dt) = 0;
    virtual void draw(sf::RenderTarget *l_target) = 0;

    virtual void updateStyle(const GUI_ElementState &l_state, const GUI_Style &l_style);
    virtual void applyStyle();

    bool isInside(const sf::Vector2f &l_point) const;
    bool isControl() const;
    bool isActive() const;
    bool needsRedraw() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getGlobalPosition() const;
    const sf::Vector2f &getSize() const;
    std::string getText() const;
    GUI_ElementState getState() const;
    GUI_ElementType getType() const;
    void setText(const std::string &l_text);
    void setActive(const bool &l_active);
    void setRedraw(bool l_redraw);
    void setState(const GUI_ElementState &l_state);
    void setPosition(const sf::Vector2f &l_pos);
    void setName(const std::string &l_name);
    void setOwner(GUI_Interface *l_owner);

    friend std::stringstream &operator >> (std::stringstream &l_stream, GUI_Element &b)
    {
        b.readIn(l_stream);
        return l_stream;
    }

protected:
    void applyTextStyle();
    void applyBgStyle();
    void applyGlyphStyle();

    void requireTexture(const std::string &l_name);
    void requireFont(const std::string &l_name);
    void releaseTexture(const std::string &l_name);
    void releaseFont(const std::string &l_name);
    void releaseResources();

    std::string m_name;
    sf::Vector2f m_position;
    ElementStyles m_style;
    GUI_Visual m_visual;
    GUI_ElementType m_type;
    GUI_ElementState m_state;
    GUI_Interface *m_owner;

    bool m_needsRedraw;
    bool m_active;
    bool m_isControl;
};
