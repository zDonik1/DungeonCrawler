#pragma once

#include "c_base.h"

class C_Controller : public C_Base
{
public:
    C_Controller() : C_Base(Component::Controller)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
    }
};
