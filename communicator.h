#pragma once

#include <vector>
#include <algorithm>
#include "observer.h"

using ObserverContainer = std::vector<Observer*>;

class Communicator
{
public:
    ~Communicator()
    {
        m_observers.clear();
    }

    bool addObserver(Observer *l_observer)
    {
        if (hasObserver(l_observer)) {
            return false;
        }

        m_observers.emplace_back(l_observer);

        return true;
    }

    bool removeObserver(Observer *l_observer)
    {
        auto observer = std::find_if(m_observers.begin(), m_observers.end(),
                                     [&l_observer] (Observer *o) {return o == l_observer;});
        if (observer == m_observers.end()) {
            return false;
        }

        m_observers.erase(observer);

        return true;
    }

    bool hasObserver(const Observer *l_observer)
    {
        return (std::find_if(m_observers.begin(), m_observers.end(),
                             [&l_observer] (Observer *o) {return o == l_observer;}) != m_observers.end());
    }

    void broadcast(const Message &l_msg)
    {
        for (auto &itr : m_observers) {
            itr->notify(l_msg);
        }
    }

private:
    ObserverContainer m_observers;
};
