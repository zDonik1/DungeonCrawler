#pragma once

#include <functional>
#include "gui_interface.h"
#include "gui_label.h"
#include "gui_scrollbar.h"
#include "gui_textfield.h"
#include "eventmanager.h"

enum class StateType;

using GUI_Interfaces = std::map<std::string, GUI_Interface*>;
using GUI_Container = std::unordered_map<StateType, GUI_Interfaces>;
using GUI_Events = std::unordered_map<StateType, std::vector<GUI_Event>>;
using GUI_Factory = std::unordered_map<GUI_ElementType, std::function<GUI_Element*(GUI_Interface*)>>;
using GUI_ElementTypes = std::unordered_map<std::string, GUI_ElementType>;

struct SharedContext;

class GUI_Manager
{
    friend class GUI_Interface;

public:
    GUI_Manager(EventManager *l_evMgr, SharedContext *l_context);
    ~GUI_Manager();

    bool addInterface(const StateType &l_state, const std::string &l_name);
    bool removeInterface(const StateType &l_state, const std::string &l_name);
    void handleClick(EventDetails *l_details);
    void handleRelease(EventDetails *l_detalis);
    void handleTextEntered(EventDetails *l_details);
    void addEvent(GUI_Event l_event);
    bool pollEvent(GUI_Event &l_event);
    void update(float l_dt);
    void render(sf::RenderWindow *l_wind);
    bool loadInterface(const StateType &l_state, const std::string &l_interface, const std::string &l_name);

    GUI_Interface *getInterface(const StateType &l_state, const std::string &l_name);
    SharedContext *getContext() const;
    void setCurrentState(const StateType &l_state);

    template<class T>
    void registerElement(const GUI_ElementType &l_id)
    {
        m_factory[l_id] = [] (GUI_Interface *l_owner) -> GUI_Element*
        {
            return new T("", l_owner);
        };
    }

private:
    GUI_Element *createElement(const GUI_ElementType &l_id, GUI_Interface *l_owner);
    GUI_ElementType stringToType(const std::string &l_string);
    bool loadStyle(const std::string &l_file, GUI_Element *l_element);

    GUI_Container m_interfaces;
    GUI_Events m_events;
    EventManager *m_eventMgr;
    SharedContext *m_context;
    StateType m_currentState;
    GUI_Factory m_factory;
    GUI_ElementTypes m_elementTypes;
};
