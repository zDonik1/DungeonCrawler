#include "gui_manager.h"
#include "statemanager.h"

GUI_Manager::GUI_Manager(EventManager *l_evMgr, SharedContext *l_context)
    : m_eventMgr(l_evMgr)
    , m_context(l_context)
    , m_currentState(StateType(0))
{
    registerElement<GUI_Label>(GUI_ElementType::Label);
    registerElement<GUI_Scrollbar>(GUI_ElementType::Scrollbar);
    registerElement<GUI_Textfield>(GUI_ElementType::Textfield);

    m_elementTypes.emplace("Label", GUI_ElementType::Label);
    m_elementTypes.emplace("Button", GUI_ElementType::Button);
    m_elementTypes.emplace("Scrollbar", GUI_ElementType::Scrollbar);
    m_elementTypes.emplace("TextField", GUI_ElementType::Textfield);
    m_elementTypes.emplace("Interface", GUI_ElementType::Window);

    m_eventMgr->addCallback(StateType(0), "Mouse_Left", &GUI_Manager::handleClick, this);
    m_eventMgr->addCallback(StateType(0), "Mouse_Left_Release", &GUI_Manager::handleRelease, this);
    m_eventMgr->addCallback(StateType(0), "Text_Entered", &GUI_Manager::handleTextEntered, this);
}

GUI_Manager::~GUI_Manager()
{
    m_eventMgr->removeCallback(StateType(0), "Mouse_Left");
    m_eventMgr->removeCallback(StateType(0), "Mouse_Left_Released");
    m_eventMgr->removeCallback(StateType(0), "Text_Entered");

    for (auto &itr : m_interfaces) {
        for (auto &itr2 : itr.second) {
            delete itr2.second;
        }
    }
}

bool GUI_Manager::addInterface(const StateType &l_state, const std::string &l_name)
{
    auto s = m_interfaces.emplace(l_state, GUI_Interfaces()).first;
    GUI_Interface *temp = new GUI_Interface(l_name, this);
    if (s->second.emplace(l_name, temp).second) {
        return true;
    }

    delete temp;
    return false;
}

bool GUI_Manager::removeInterface(const StateType &l_state, const std::string &l_name)
{
    auto s = m_interfaces.find(l_state);
    if (s == m_interfaces.end()) {
        return false;
    }

    auto i = s->second.find(l_name);
    if (i == s->second.end()) {
        return false;
    }

    delete i->second;
    return s->second.erase(l_name);
}

void GUI_Manager::handleClick(EventDetails *l_details)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) {
        return;
    }

    sf::Vector2i mousePos = m_eventMgr->getMousePos(m_context->m_wind->getRenderWindow());
    for (auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr) {
        if (!itr->second->isInside(sf::Vector2f(mousePos))) {
            continue;
        }

        if (!itr->second->isActive()) {
            return;
        }

        itr->second->onClick(sf::Vector2f(mousePos));
        itr->second->focus();
        if (itr->second->isBeingMoved()) {
            itr->second->beginMoving();
        }

        return;
    }
}

void GUI_Manager::handleRelease(EventDetails *l_detalis)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) {
        return;
    }

    for (auto &itr : state->second) {
        GUI_Interface *i = itr.second;
        if (!i->isActive()) {
            continue;
        }

        if (i->getState() == GUI_ElementState::Clicked) {
            i->onRelease();
        }

        if (i->isBeingMoved()) {
            i->stopMoving();
        }
    }
}

void GUI_Manager::handleTextEntered(EventDetails *l_details)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) {
        return;
    }

    for (auto &itr : state->second) {
        if (!itr.second->isActive()) {
            continue;
        }

        if (!itr.second->isFocused()) {
            continue;
        }

        itr.second->onTextEntered(l_details->m_textEntered);
        return;
    }
}

void GUI_Manager::addEvent(GUI_Event l_event)
{
    m_events[m_currentState].push_back(l_event);
}

bool GUI_Manager::pollEvent(GUI_Event &l_event)
{
    if (m_events[m_currentState].empty()) {
        return false;
    }

    l_event = m_events[m_currentState].back();
    m_events[m_currentState].pop_back();
    return true;
}

void GUI_Manager::update(float l_dt)
{
    sf::Vector2i mousePos = m_eventMgr->getMousePos(m_context->m_wind->getRenderWindow());

    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) {
        return;
    }

    for (auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr) {
        GUI_Interface *i = itr->second;
        if (!i->isActive()) {
            continue;
        }

        i->update(l_dt);
        if (i->isBeingMoved()) {
            continue;
        }

        if (i->isInside(sf::Vector2f(mousePos))) {
            if (i->getState() == GUI_ElementState::Neutral) {
                i->onHover(sf::Vector2f(mousePos));
            }

            return;
        }
        else if (i->getState() == GUI_ElementState::Focused) {
            i->onLeave();
        }
    }
}

void GUI_Manager::render(sf::RenderWindow *l_wind)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end()) {
        return;
    }

    for (auto &itr : state->second) {
        GUI_Interface *i = itr.second;
        if (!i->isActive()) {
            continue;
        }

        if (i->needsRedraw()) {
            i->redraw();
        }
        if (i->needsContentRedraw()) {
            i->redrawContent();
        }
        if (i->needsControlRedraw()) {
            i->redrawControls();
        }

        i->draw(l_wind);
    }
}

bool GUI_Manager::loadInterface(const StateType &l_state, const std::string &l_interface, const std::string &l_name)
{
    std::ifstream file;
    file.open(Utils::getWorkingDirectory() + "media/GUI_Interfaces/" + l_interface);
    std::string interfaceName;

    if (!file.is_open()){
        std::cout << "! Failed to load: " << l_interface << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)){
        if (line[0] == '|') {
            continue;
        }

        std::stringstream keystream(line);
        std::string key;
        keystream >> key;
        if (key == "Interface") {
            std::string style;
            keystream >> interfaceName >> style;
            if (!addInterface(l_state, l_name)) {
                std::cout << "Failed adding interface: " << l_name << std::endl;
                return false;
            }

            GUI_Interface *i = getInterface(l_state, l_name);
            keystream >> *i;
            if (!loadStyle(style, i)) {
                std::cout << "Failed loading style file: " << style << " for interface " << l_name << std::endl;
            }

            i->setContentSize(i->getSize());
        }
        else if (key == "Element") {
            if (interfaceName == "") {
                std::cout << "Error: 'Element' outside or before declaration of 'Interface'!" << std::endl;
                continue;
            }

            std::string type;
            std::string name;
            sf::Vector2f position;
            std::string style;
            keystream >> type >> name >> position.x >> position.y >> style;
            GUI_ElementType eType = stringToType(type);
            if (eType == GUI_ElementType::None) {
                std::cout << "Unknown element('" << name << "') type: " << type << std::endl;
                continue;
            }

            GUI_Interface *i = getInterface(l_state, l_name);
            if (!i) {
                continue;
            }

            if (!i->addElement(eType, name)) {
                continue;
            }

            GUI_Element *e = i->getElement(name);
            keystream >> *e;
            e->setPosition(position);
            if (!loadStyle(style, e)) {
                std::cout << "Failed loading style file: " << style << " for element " << name << std::endl;
                continue;
            }
        }
    }

    file.close();
    return true;
}

GUI_Interface *GUI_Manager::getInterface(const StateType &l_state, const std::string &l_name)
{
    auto s = m_interfaces.find(l_state);
    if (s == m_interfaces.end()) {
        return nullptr;
    }

    auto i = s->second.find(l_name);
    return (i != s->second.end() ? i->second : nullptr);
}

SharedContext *GUI_Manager::getContext() const
{
    return m_context;
}

void GUI_Manager::setCurrentState(const StateType &l_state)
{
    if (m_currentState == l_state) {
        return;
    }

    handleRelease(nullptr);
    m_currentState = l_state;
}

GUI_Element *GUI_Manager::createElement(const GUI_ElementType &l_id, GUI_Interface *l_owner)
{
    if (l_id == GUI_ElementType::Window) {
        return new GUI_Interface("", this);
    }

    auto f = m_factory.find(l_id);
    if (f != m_factory.end()) {
        return f->second(l_owner);
    }
    else {
        return nullptr;
    }
}

GUI_ElementType GUI_Manager::stringToType(const std::string &l_string)
{
    return m_elementTypes[l_string];
}

bool GUI_Manager::loadStyle(const std::string &l_file, GUI_Element *l_element)
{
    std::ifstream file;
    file.open(Utils::getWorkingDirectory() + "media/GUI_Styles/" + l_file);

    std::string currentState;
    GUI_Style parentStyle;
    GUI_Style temporaryStyle;

    if (!file.is_open()){
        std::cout << "! Failed to load: " << l_file << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)){
        if (line[0] == '|') {
            continue;
        }

        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "") {
            continue;
        }

        if (type == "State") {
            if (currentState != "") {
                std::cout << "Error: 'State' keyword found inside another state!" << std::endl;
                continue;
            }

            keystream >> currentState;
        }
        else if (type == "/State") {
            if (currentState == "") {
                std::cout << "Error: '/State' keyword found prior to 'State'!" << std::endl;
                continue;
            }

            GUI_ElementState state = GUI_ElementState::Neutral;
            if (currentState == "Hover") {
                state = GUI_ElementState::Focused;
            }
            else if (currentState == "Clicked") {
                state = GUI_ElementState::Clicked;
            }

            if (state == GUI_ElementState::Neutral) {
                parentStyle = temporaryStyle;
                l_element->updateStyle(GUI_ElementState::Neutral, temporaryStyle);
                l_element->updateStyle(GUI_ElementState::Focused, temporaryStyle);
                l_element->updateStyle(GUI_ElementState::Clicked, temporaryStyle);
            }
            else {
                l_element->updateStyle(state, temporaryStyle);
            }

            temporaryStyle = parentStyle;
            currentState = "";
        }
        else {
            if (currentState == "") {
                std::cout << "Error: '" << type << "' keyword found outside of a state!" << std::endl;
                continue;
            }

            if (type == "Size") {
                keystream >> temporaryStyle.m_size.x >> temporaryStyle.m_size.y;
            }
            else if (type == "BgColor") {
                int r, g, b, a = 0;
                keystream >> r >> g >> b >> a;
                temporaryStyle.m_backgroundColor = sf::Color(r, g, b, a);
            }
            else if (type == "BgImage") {
                keystream >> temporaryStyle.m_backgroundImage;
            }
            else if (type == "BgImageColor") {
                int r, g, b, a = 0;
                keystream >> r >> g >> b >> a;
                temporaryStyle.m_backgroundImageColor = sf::Color(r, g, b, a);
            }
            else if (type == "TextColor") {
                int r, g, b, a = 0;
                keystream >> r >> g >> b >> a;
                temporaryStyle.m_textColor = sf::Color(r, g, b, a);
            }
            else if (type == "TextSize") {
                keystream >> temporaryStyle.m_textSize;
            }
            else if (type == "TextOriginCenter") {
                temporaryStyle.m_textCenterOrigin = true;
            }
            else if (type == "Font") {
                keystream >> temporaryStyle.m_textFont;
            }
            else if (type == "TextPadding") {
                keystream >> temporaryStyle.m_textPadding.x >> temporaryStyle.m_textPadding.y;
            }
            else if (type == "ElementColor") {
                int r, g, b, a = 0;
                keystream >> r >> g >> b >> a;
                temporaryStyle.m_elementColor = sf::Color(r, g, b, a);
            }
            else if (type == "Glyph") {
                keystream >> temporaryStyle.m_glyph;
            }
            else if (type == "GlyphPadding") {
                keystream >> temporaryStyle.m_glyphPadding.x >> temporaryStyle.m_glyphPadding.y;
            }
            else {
                std::cout << "Error: style tag '" << type << "' is uknown!" << std::endl;
            }
        }
    }

    file.close();
    return true;
}
