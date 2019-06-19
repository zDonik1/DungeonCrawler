#pragma once

#include <sstream>
#include "esc_types.h"

class C_Base
{
public:
    C_Base(const Component &l_type)
        : m_type(l_type)
    {
    }

    virtual ~C_Base()
    {
    }

    Component getType()
    {
        return m_type;
    }

    friend std::stringstream &operator >> (std::stringstream &l_stream, C_Base &b)
    {
        b.readIn(l_stream);
        return l_stream;
    }

    virtual void readIn(std::stringstream &l_stream) = 0;

protected:
    Component m_type;
};
