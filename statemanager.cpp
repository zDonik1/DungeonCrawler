#include "statemanager.h"
#include "state_intro.h"
#include "state_mainmenu.h"
#include "state_game.h"
#include "state_paused.h"
#include "state_gameover.h"

StateManager::StateManager(SharedContext* l_shared) : m_context(l_shared)
{
    registerState<State_Intro>(StateType::Intro);
    registerState<State_MainMenu>(StateType::MainMenu);
    registerState<State_Game>(StateType::Game);
    registerState<State_Paused>(StateType::Paused);
    registerState<State_GameOver>(StateType::GameOver);
}

StateManager::~StateManager()
{
    for (auto& itr : m_states) {
        itr.second->onDestroy();
        delete itr.second;
    }
}

void StateManager::draw()
{
    if (m_states.empty())
        return;

    if (m_states.back().second->isTransparent() && m_states.size() > 1) {
        auto itr = m_states.end();
        while (itr != m_states.begin()) {
            if (itr != m_states.end()) {
                if (!itr->second->isTransparent())
                    break;
            }

            --itr;
        }

        for (; itr != m_states.end(); ++itr) {
            m_context->m_wind->getRenderWindow()->setView(itr->second->getView());

            itr->second->draw();
        }
    }
    else {
        m_states.back().second->draw();
    }
}

void StateManager::update(const sf::Time &l_time)
{
    if (m_states.empty())
        return;

    if (m_states.back().second->isTranscendent() && m_states.size() > 1) {
        auto itr = m_states.end();
        while (itr != m_states.begin()) {
            if (itr != m_states.end()) {
                if (!itr->second->isTranscendent())
                    break;
            }

            --itr;
        }

        for (; itr != m_states.end(); ++itr) {
            itr->second->update(l_time);
        }
    }
    else {
        m_states.back().second->update(l_time);
    }
}

SharedContext* StateManager::getContext()
{
    return m_context;
}

bool StateManager::hasState(const StateType &l_type)
{
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_type) {
            auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), l_type);

            if (removed == m_toRemove.end())
                return true;

            return false;
        }
    }

    return false;
}

void StateManager::remove(const StateType &l_type)
{
    m_toRemove.emplace_back(l_type);
}

// processRequests function has to be called at the end of game loop and removes all states that had to be removed
void StateManager::processRequests()
{
    while (m_toRemove.begin() != m_toRemove.end()) {
        removeState(*m_toRemove.begin());
        m_toRemove.erase(m_toRemove.begin());
    }
}

void StateManager::switchTo(const StateType &l_type)
{
    m_context->m_eventManager->setCurrentState(l_type);
    m_context->m_guiManager->setCurrentState(l_type);
    m_context->m_soundManager->changeState(l_type);

    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_type) {
            m_states.back().second->deactivate();
            StateType tmp_type = itr->first;
            BaseState* tmp_state = itr->second;

            m_states.erase(itr);
            m_states.emplace_back(tmp_type, tmp_state);
            tmp_state->activate();

            m_context->m_wind->getRenderWindow()->setView(tmp_state->getView());

            return;
        }
    }

    if (!m_states.empty())
        m_states.back().second->deactivate();

    createState(l_type);
    m_states.back().second->activate();

    m_context->m_wind->getRenderWindow()->setView(m_states.back().second->getView());
}

// PRIVATE MEMBER FUNCITONS

void StateManager::createState(const StateType &l_type)
{
    auto newState = m_stateFactory.find(l_type);

    if (newState == m_stateFactory.end())
        return;

    BaseState* state = newState->second();
    state->m_view = m_context->m_wind->getRenderWindow()->getDefaultView();

    m_states.emplace_back(l_type, state);
    state->onCreate();
}

//
void StateManager::removeState(const StateType &l_type)
{
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
        if (itr->first == l_type) {
            itr->second->onDestroy();
            delete itr->second;
            m_states.erase(itr);
            m_context->m_soundManager->removeState(l_type);
            return;
        }
    }
}
