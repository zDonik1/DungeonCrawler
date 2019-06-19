#pragma once

#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "eventmanager.h"

class Window
{
public:
	Window();
	Window(const std::string& title, const sf::Vector2u& size);
	~Window();

    void beginDraw();
    void endDraw();

    void update();

    bool isDone();
    bool isFullscreen();
    bool isFocused();

    void toggleFullscreen(EventDetails* l_details);
    void close(EventDetails* l_details = nullptr);

    sf::RenderWindow* getRenderWindow();
    EventManager* getEventManager();
    sf::Vector2u getWindowSize();
    sf::FloatRect getViewSpace();

private:
    void setup(const std::string& title, const sf::Vector2u& size);
    void create();

	sf::RenderWindow m_window;
	EventManager m_eventManager;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isDone;
	bool m_isFullscreen;
	bool m_isFocused;
};
