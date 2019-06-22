#include "window.h"

Window::Window()
{
    setup("Window", sf::Vector2u(640,480));
}

Window::Window(const std::string& title, const sf::Vector2u& size)
{
    setup(title,size);
}

Window::~Window()
{
    m_window.close();
}

void Window::setup(const std::string& title, const sf::Vector2u& size)
{
	m_windowTitle = title;
	m_windowSize = size;
	m_isFullscreen = false;
	m_isDone = false;
	m_isFocused = true;

    m_eventManager.addCallback(StateType(0),"Fullscreen_toggle",&Window::toggleFullscreen,this);
    m_eventManager.addCallback(StateType(0),"Window_close", &Window::close, this);

    create();
}

void Window::create()
{
	sf::Uint32 style = sf::Style::Default;
	if(m_isFullscreen){ style = sf::Style::Fullscreen; }

	m_window.create(sf::VideoMode(m_windowSize.x,m_windowSize.y,32),m_windowTitle,style);

    m_window.setFramerateLimit(60);
}

void Window::beginDraw()
{
    m_window.clear(sf::Color::Black);
}

void Window::endDraw()
{
    m_window.display();
}

bool Window::isDone()
{
    return m_isDone;
}

bool Window::isFullscreen()
{
    return m_isFullscreen;
}

bool Window::isFocused()
{
    return m_isFocused;
}

sf::RenderWindow* Window::getRenderWindow()
{
    return &m_window;
}

EventManager* Window::getEventManager()
{
    return &m_eventManager;
}

sf::Vector2u Window::getWindowSize()
{
    return m_windowSize;
}

sf::FloatRect Window::getViewSpace()
{
    sf::Vector2f viewCenter = m_window.getView().getCenter();
    sf::Vector2f viewSize = m_window.getView().getSize();
    sf::Vector2f viewSizeHalf(viewSize.x / 2.0f, viewSize.y / 2.0f);
    sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
    return viewSpace;
}

void Window::toggleFullscreen(EventDetails* l_details)
{
	m_isFullscreen = !m_isFullscreen;
	m_window.close();
    create();
}

void Window::close(EventDetails* l_details)
{
    m_isDone = true;
}

void Window::update()
{
	sf::Event event;

	while(m_window.pollEvent(event)){
        if (event.type == sf::Event::LostFocus) {
            m_isFocused = false;
            m_eventManager.setFocus(false);
        }
        else if (event.type == sf::Event::GainedFocus) {
            m_isFocused = true;
            m_eventManager.setFocus(true);
        }
        m_eventManager.handleEvent(event);
	}

    m_eventManager.update();
}
