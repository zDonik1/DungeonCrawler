#pragma once

#include "basestate.h"
#include "sharedcontext.h"

enum class StateType
{
    Intro = 1, MainMenu, Game, Paused, GameOver, Credits
};

using StateContainer = std::vector<std::pair<StateType, BaseState*>>;

using TypeContainer = std::vector<StateType>;

using StateFactory = std::unordered_map<StateType, std::function<BaseState* (void)>>;

class StateManager
{
public:
    StateManager(SharedContext* l_shared);
    ~StateManager();

    void update(const sf::Time& l_time);
    void draw();

    void processRequests();

    SharedContext* getContext();
    bool hasState(const StateType& l_type);

    void switchTo(const StateType& l_type);
    void remove(const StateType& l_type);

private:
    void createState(const StateType& l_type);
    void removeState(const StateType& l_type);

    // registerState function should be called in StateManager constructor to be registered as a valid state
    //   state is registered by creating a function that returns the instance of the state
    template<class T>
    void registerState(const StateType& l_type)
    {
        m_stateFactory[l_type] = [this] () -> BaseState*
        {
            return new T(this);
        };
    }


    SharedContext* m_context;
    StateContainer m_states;
    TypeContainer m_toRemove;
    StateFactory m_stateFactory;
};
