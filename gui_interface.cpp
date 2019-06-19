#include "gui_interface.h"
#include "gui_manager.h"
#include "sharedcontext.h"

GUI_Interface::GUI_Interface(const std::string &l_name, GUI_Manager *l_guiManager)
    : GUI_Element(l_name, GUI_ElementType::Window, this)
    , m_parent(nullptr)
    , m_guiManager(l_guiManager)
    , m_showTitleBar(false)
    , m_movable(false)
    , m_beingMoved(false)
    , m_focused(false)
    , m_scrollHorizontal(0)
    , m_scrollVertical(0)
    , m_contentRedraw(true)
    , m_controlRedraw(true)
{
    m_backdropTexture = new sf::RenderTexture();
    m_contentTexture = new sf::RenderTexture();
    m_controlTexture = new sf::RenderTexture();
}

GUI_Interface::~GUI_Interface()
{
    delete m_backdropTexture;
    delete m_contentTexture;
    delete m_controlTexture;
    for (auto &itr : m_elements) {
        delete itr.second;
    }
}

bool GUI_Interface::isInside(const sf::Vector2f &l_point) const
{
    if (GUI_Element::isInside(l_point)) {
        return true;
    }

    return m_titleBar.getGlobalBounds().contains(l_point);
}

bool GUI_Interface::isBeingMoved() const
{
    return m_beingMoved;
}

bool GUI_Interface::isFocused() const
{
    return m_focused;
}

bool GUI_Interface::needsContentRedraw() const
{
    return m_contentRedraw;
}

bool GUI_Interface::needsControlRedraw() const
{
    return m_controlRedraw;
}

const sf::Vector2f &GUI_Interface::getPadding() const
{
    return m_elementPadding;
}

sf::Vector2f GUI_Interface::getGlobalPosition() const
{
    sf::Vector2f pos = m_position;
    GUI_Interface *i = m_parent;
    while (i) {
        pos += i->getPosition();
        i = i->m_parent;
    }
    return pos;
}

GUI_Element *GUI_Interface::getElement(const std::string &l_name) const
{
    auto itr = m_elements.find(l_name);
    return (itr != m_elements.end() ? itr->second : nullptr);
}

const sf::Vector2f &GUI_Interface::getContentSize() const
{
    return m_contentSize;
}

GUI_Manager *GUI_Interface::getManager() const
{
    return m_guiManager;
}

void GUI_Interface::setPosition(const sf::Vector2f &l_pos)
{
    GUI_Element::setPosition(l_pos);
    m_backdrop.setPosition(l_pos);
    m_content.setPosition(l_pos);
    m_control.setPosition(l_pos);
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
}

void GUI_Interface::focus()
{
    m_focused = true;
}

bool GUI_Interface::addElement(const GUI_ElementType &l_type, const std::string &l_name)
{
    if (m_elements.find(l_name) != m_elements.end()) {
        return false;
    }

    GUI_Element *element = nullptr;
    element = m_guiManager->createElement(l_type, this);
    if (!element) {
        return false;
    }

    element->setName(l_name);
    element->setOwner(this);
    m_elements.emplace(l_name, element);
    m_contentRedraw = true;
    m_controlRedraw = true;
    return true;
}

bool GUI_Interface::removeElement(const std::string &l_name)
{
    auto itr = m_elements.find(l_name);
    if (itr == m_elements.end()) {
        return false;
    }

    delete itr->second;
    m_elements.erase(itr);
    m_contentRedraw = true;
    m_controlRedraw = true;
    adjustContentSize();
    return true;
}

void GUI_Interface::readIn(std::stringstream &l_stream)
{
    std::string movableState;
    std::string titleShow;
    std::string title;
    l_stream >> m_elementPadding.x >> m_elementPadding.y >> movableState >> titleShow;
    Utils::readQuotedString(l_stream, title);
    m_visual.m_text.setString(title);
    if (movableState == "Movable") {
        m_movable = true;
    }
    if (titleShow == "Title") {
        m_showTitleBar = true;
    }
}

void GUI_Interface::onClick(const sf::Vector2f &l_mousePos)
{
    defocusTextFields();

    if (m_titleBar.getGlobalBounds().contains(l_mousePos) && m_movable && m_showTitleBar) {
        m_beingMoved = true;
    }
    else {
        GUI_Event event;
        event.m_type = GUI_EventType::Click;
        event.m_interface = m_name.c_str();
        event.m_element = "";
        event.m_clickCoords.x = l_mousePos.x;
        event.m_clickCoords.y = l_mousePos.y;
        m_guiManager->addEvent(event);
        for (auto &itr : m_elements) {
            if (!itr.second->isInside(l_mousePos)) {
                continue;
            }

            itr.second->onClick(l_mousePos);
            event.m_element = itr.second->m_name.c_str();
            m_guiManager->addEvent(event);
        }

        setState(GUI_ElementState::Clicked);
    }
}

void GUI_Interface::onRelease()
{
    GUI_Event event;
    event.m_type = GUI_EventType::Release;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    m_guiManager->addEvent(event);
    for (auto &itr : m_elements) {
        if (itr.second->getState() != GUI_ElementState::Clicked) {
            continue;
        }

        itr.second->onRelease();
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->addEvent(event);
    }

    setState(GUI_ElementState::Neutral);
}

void GUI_Interface::onTextEntered(const char &l_char)
{
    for (auto &itr : m_elements) {
        if (itr.second->getType() != GUI_ElementType::Textfield) {
            continue;
        }

        if (itr.second->getState() != GUI_ElementState::Clicked) {
            continue;
        }

        if (l_char == 8) { // Backspace
            const auto &text = itr.second->getText();
            itr.second->setText(text.substr(0, text.length() - 1));
            return;
        }

        if (l_char < 32 || l_char > 126) {
            return;
        }

        std::string text = itr.second->getText();
        text.push_back(l_char);
        itr.second->setText(text);
        return;
    }
}

void GUI_Interface::defocusTextFields()
{
    GUI_Event event;
    event.m_type = GUI_EventType::Release;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    for (auto &itr : m_elements) {
        if (itr.second->getType() != GUI_ElementType::Textfield) {
            continue;
        }

        itr.second->setState(GUI_ElementState::Neutral);
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->addEvent(event);
    }
}

void GUI_Interface::onHover(const sf::Vector2f &l_mousePos)
{
    GUI_Event event;
    event.m_type = GUI_EventType::Hover;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    event.m_clickCoords.x = l_mousePos.x;
    event.m_clickCoords.y = l_mousePos.y;
    m_guiManager->addEvent(event);
    setState(GUI_ElementState::Focused);
}

void GUI_Interface::onLeave()
{
    GUI_Event event;
    event.m_type = GUI_EventType::Leave;
    event.m_interface = m_name.c_str();
    event.m_element = "";
    m_guiManager->addEvent(event);
    setState(GUI_ElementState::Neutral);
}

void GUI_Interface::beginMoving()
{
    if (!m_showTitleBar || !m_movable) {
        return;
    }

    m_beingMoved = true;
    SharedContext *context = m_guiManager->getContext();
    m_moveMouseLast = sf::Vector2f(context->m_eventManager->getMousePos(context->m_wind->getRenderWindow()));
}

void GUI_Interface::stopMoving()
{
    m_beingMoved = false;
}

void GUI_Interface::applyStyle()
{
    GUI_Element::applyStyle();

    m_visual.m_backgroundSolid.setPosition(0.f, 0.f);
    m_visual.m_backgroundImage.setPosition(0.f, 0.f);
    m_titleBar.setSize(sf::Vector2f(m_style[m_state].m_size.x, 16.f));
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_titleBar.setFillColor(m_style[m_state].m_elementColor);
    m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
    m_visual.m_glyph.setPosition(m_titleBar.getPosition() + m_style[m_state].m_glyphPadding);
}

void GUI_Interface::redraw()
{
    if (m_backdropTexture->getSize().x != m_style[m_state].m_size.x ||
            m_backdropTexture->getSize().y != m_style[m_state].m_size.y)
    {
        m_backdropTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
    applyStyle();
    m_backdropTexture->draw(m_visual.m_backgroundSolid);

    if (m_style[m_state].m_backgroundImage != "") {
        m_backdropTexture->draw(m_visual.m_backgroundImage);
    }

    m_backdropTexture->display();
    m_backdrop.setTexture(m_backdropTexture->getTexture());
    m_backdrop.setTextureRect(sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    setRedraw(false);
}

void GUI_Interface::redrawContent()
{
    if (m_contentTexture->getSize().x != m_contentSize.x ||
            m_contentTexture->getSize().y != m_contentSize.y)
    {
        m_contentTexture->create(m_contentSize.x, m_contentSize.y);
    }

    m_contentTexture->clear(sf::Color(0, 0, 0, 0));

    for (auto &itr : m_elements) {
        GUI_Element *element = itr.second;
        if (!element->isActive() || element->isControl()) {
            continue;
        }

        element->applyStyle();
        element->draw(m_contentTexture);
        element->setRedraw(false);
    }

    m_contentTexture->display();
    m_content.setTexture(m_contentTexture->getTexture());

    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical,
                                         m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_contentRedraw = false;
}

void GUI_Interface::redrawControls()
{
    if (m_controlTexture->getSize().x != m_style[m_state].m_size.x ||
            m_controlTexture->getSize().y != m_style[m_state].m_size.y)
    {
        m_controlTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }

    m_controlTexture->clear(sf::Color(0, 0, 0, 0));

    for (auto &itr : m_elements) {
        GUI_Element *element = itr.second;
        if (!element->isActive() || !element->isControl()) {
            continue;
        }

        element->applyStyle();
        element->draw(m_controlTexture);
        element->setRedraw(false);
    }

    m_controlTexture->display();
    m_control.setTexture(m_controlTexture->getTexture());
    m_control.setTextureRect(sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_controlRedraw = false;
}

void GUI_Interface::updateScrollHorizontal(unsigned int l_percent)
{
    if (l_percent > 100) {
        return;
    }

    m_scrollHorizontal = ((m_contentSize.x - getSize().x) / 100) * l_percent;
    sf::IntRect rect = m_content.getTextureRect();
    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::updateScrollVertical(unsigned int l_percent)
{
    if (l_percent > 100) {
        return;
    }

    m_scrollVertical = ((m_contentSize.y - getSize().y) / 100) * l_percent;
    sf::IntRect rect = m_content.getTextureRect();
    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::adjustContentSize(const GUI_Element *l_reference)
{
    if (l_reference) {
        sf::Vector2f bottomRight = l_reference->getPosition() + l_reference->getSize();
        if (bottomRight.x > m_contentSize.x) {
            m_contentSize.x = bottomRight.x;
            m_controlRedraw = true;
        }

        if (bottomRight.y > m_contentSize.y) {
            m_contentSize.y = bottomRight.y;
            m_controlRedraw = true;
        }

        return;
    }

    sf::Vector2f farthest = getSize();

    for (auto &itr : m_elements) {
        GUI_Element *element = itr.second;
        if (!element->isActive() || element->isControl()) {
            continue;
        }

        sf::Vector2f bottomRight = element->getPosition() + element->getSize();
        if (bottomRight.x > farthest.x) {
            farthest.x = bottomRight.x;
            m_controlRedraw = true;
        }
        if (bottomRight.y > farthest.y) {
            farthest.y = bottomRight.y;
            m_controlRedraw = true;
        }
    }

    setContentSize(farthest);
}

void GUI_Interface::setContentSize(const sf::Vector2f &l_vec)
{
    m_contentSize = l_vec;
}

void GUI_Interface::update(float l_dt)
{
    sf::Vector2f mousePos = sf::Vector2f(m_guiManager->getContext()->m_eventManager->getMousePos(
                                             m_guiManager->getContext()->m_wind->getRenderWindow()));

    if (m_beingMoved && m_moveMouseLast != mousePos) {
        sf::Vector2f difference = mousePos - m_moveMouseLast;
        m_moveMouseLast = mousePos;
        sf::Vector2f newPosition = m_position + difference;
        setPosition(newPosition);
    }

    for (auto &itr : m_elements) {
        if (itr.second->needsRedraw()) {
            if (itr.second->isControl()) {
                m_controlRedraw = true;
            }
            else {
                m_contentRedraw = true;
            }
        }

        if (!itr.second->isActive()) {
            continue;
        }

        itr.second->update(l_dt);
        if (m_beingMoved) {
            continue;
        }

        GUI_Event event;
        event.m_interface = m_name.c_str();
        event.m_element = itr.second->m_name.c_str();
        event.m_clickCoords.x = mousePos.x;
        event.m_clickCoords.y = mousePos.y;
        if (isInside(mousePos) && itr.second->isInside(mousePos) && !m_titleBar.getGlobalBounds().contains(mousePos)) {
            if (itr.second->getState() != GUI_ElementState::Neutral) {
                continue;
            }

            itr.second->onHover(mousePos);
            event.m_type = GUI_EventType::Hover;
        }
        else if (itr.second->getState() == GUI_ElementState::Focused) {
            itr.second->onLeave();
            event.m_type = GUI_EventType::Leave;
        }

        m_guiManager->addEvent(event);
    }
}

void GUI_Interface::draw(sf::RenderTarget *l_target)
{
    l_target->draw(m_backdrop);
    l_target->draw(m_content);
    l_target->draw(m_control);

    if (!m_showTitleBar) {
        return;
    }

    l_target->draw(m_titleBar);
    l_target->draw(m_visual.m_text);
}

