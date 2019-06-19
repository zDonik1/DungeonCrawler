#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utilities.h"
#include "gui_event.h"

enum class EventType
{
    KeyDown = sf::Event::KeyPressed,
    KeyUp = sf::Event::KeyReleased,
    MButtonDown = sf::Event::MouseButtonPressed,
    MButtonUp = sf::Event::MouseButtonReleased,
    MouseWheel = sf::Event::MouseWheelMoved,
    WindowResized = sf::Event::Resized,
    GainedFocus = sf::Event::GainedFocus,
    LostFocus = sf::Event::LostFocus,
    MouseEntered = sf::Event::MouseEntered,
    MouseLeft = sf::Event::MouseLeft,
    Closed = sf::Event::Closed,
    TextEntered = sf::Event::TextEntered,
    Keyboard = sf::Event::Count + 1,
    Mouse,
    Joystick,
    GUI_Click,
    GUI_Release,
    GUI_Hover,
    GUI_Leave
};

struct EventInfo
{
    EventInfo()
    {
        m_code = 0;
    }
    EventInfo(int l_event)
    {
        m_code = l_event;
    }
    EventInfo(GUI_Event l_guiEvent)
    {
        m_gui = l_guiEvent;
    }

    union
    {
        int m_code;
        GUI_Event m_gui;
    };
};

struct EventDetails
{
    EventDetails(const std::string& l_bindName) : m_name(l_bindName)
    {
        clear();
    }

    void clear()
    {
        m_size = sf::Vector2i(0, 0);
        m_textEntered = 0;
        m_mouse = sf::Vector2i(0, 0);
        m_mouseWheelDelta = 0;
        m_keyCode = -1;
        m_guiInterface = "";
        m_guiElement = "";
        m_guiEventType = GUI_EventType::None;
    }

    std::string m_name;
    sf::Vector2i m_size;
    sf::Uint32 m_textEntered;
    sf::Vector2i m_mouse;
    int m_mouseWheelDelta;
    int m_keyCode;
    std::string m_guiInterface;
    std::string m_guiElement;
    GUI_EventType m_guiEventType;
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding
{
    Binding(const std::string& l_name)
        : m_name(l_name)
        , c(0)
        , m_details(l_name)
    {
    }

    ~Binding()
    {
        for (auto itr = m_events.begin(); itr != m_events.end(); ++itr) {
            if (itr->first == EventType::GUI_Click ||
                    itr->first == EventType::GUI_Release ||
                    itr->first == EventType::GUI_Hover ||
                    itr->first == EventType::GUI_Leave)
            {
                delete [] itr->second.m_gui.m_interface;
                delete [] itr->second.m_gui.m_element;
            }
        }
    }

    void bindEvent(EventType l_type, EventInfo l_info = EventInfo())
    {
        m_events.emplace_back(l_type, l_info);
    }

    Events m_events;
    std::string m_name;
    int c; // used to count the number of events that have matched

    EventDetails m_details;
};

using Bindings = std::unordered_map<std::string, Binding*>;

using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;

enum class StateType;

using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager
{
public:
    EventManager();
    ~EventManager();

    bool addBinding(Binding* l_binding);
    bool removeBinding(std::string l_name);

    sf::Vector2i getMousePos(sf::RenderWindow* l_wind = nullptr)
    {
        return (l_wind ? sf::Mouse::getPosition(*l_wind) : sf::Mouse::getPosition());
    }

    void setCurrentState(StateType l_state);
    void setFocus(const bool& l_focus);

    template<class T>
    bool addCallback(StateType l_state, const std::string& l_name, void (T::*l_func)(EventDetails*), T* l_instance)
    {
        auto itr = m_callbacks.emplace(l_state, CallbackContainer()).first;
        auto temp = std::bind(l_func, l_instance, std::placeholders::_1);

        return itr->second.emplace(l_name, temp).second;
    }

    bool removeCallback(StateType l_state, const std::string& l_name)
    {
        auto itr = m_callbacks.find(l_state);
        if (itr == m_callbacks.end())
            return false;

        auto itr2 = itr->second.find(l_name);
        if(itr2 == itr->second.end())
            return false;

        itr->second.erase(l_name);
        return true;
    }

    void handleEvent(sf::Event &l_event);
    void handleEvent(GUI_Event &l_event);
    void update();

private:
    void loadBindings();

    StateType m_currentState;
    Bindings m_bindings;
    Callbacks m_callbacks;

    bool m_hasFocus;
};
