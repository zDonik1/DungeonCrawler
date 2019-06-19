#pragma once

#include "gui_element.h"

using Elements = std::unordered_map<std::string, GUI_Element*>;

class GUI_Manager;

class GUI_Interface : public GUI_Element
{
    friend class GUI_Element;
    friend class GUI_Manager;

public:
    GUI_Interface(const std::string &l_name, GUI_Manager *l_guiManager);
    ~GUI_Interface();

    bool isInside(const sf::Vector2f &l_point) const;
    bool isBeingMoved() const;
    bool isFocused() const;
    bool needsContentRedraw() const;
    bool needsControlRedraw() const;
    const sf::Vector2f &getPadding() const;
    sf::Vector2f getGlobalPosition() const;
    GUI_Element *getElement(const std::string &l_name) const;
    const sf::Vector2f &getContentSize() const;
    GUI_Manager *getManager() const;
    void setPosition(const sf::Vector2f &l_pos);
    void focus();

    bool addElement(const GUI_ElementType &l_type, const std::string &l_name);
    bool removeElement(const std::string &l_name);
    void readIn(std::stringstream &l_stream);
    void onClick(const sf::Vector2f &l_mousePos);
    void onRelease();
    void onTextEntered(const char &l_char);
    void onHover(const sf::Vector2f &l_mousePos);
    void onLeave();
    void beginMoving();
    void stopMoving();
    void applyStyle();
    void redraw();
    void redrawContent();
    void redrawControls();
    void updateScrollHorizontal(unsigned int l_percent);
    void updateScrollVertical(unsigned int l_percent);

    void update(float l_dt);
    void draw(sf::RenderTarget *l_target);

private:
    void defocusTextFields();
    Elements m_elements;
    sf::Vector2f m_elementPadding;
    GUI_Interface *m_parent;
    GUI_Manager *m_guiManager;

    sf::RenderTexture *m_backdropTexture;
    sf::Sprite m_backdrop;

    // Movement
    sf::RectangleShape m_titleBar;
    sf::Vector2f m_moveMouseLast;
    bool m_showTitleBar;
    bool m_movable;
    bool m_beingMoved;
    bool m_focused;

    // Variable size
    void adjustContentSize(const GUI_Element *l_reference = nullptr);
    void setContentSize(const sf::Vector2f &l_vec);

    sf::RenderTexture *m_contentTexture;
    sf::Sprite m_content;
    sf::Vector2f m_contentSize;
    int m_scrollHorizontal;
    int m_scrollVertical;
    bool m_contentRedraw;

    // Control layer
    sf::RenderTexture *m_controlTexture;
    sf::Sprite m_control;
    bool m_controlRedraw;
};
