#pragma once

#include "c_base.h"

class C_SoundListener : public C_Base
{
public:
    C_SoundListener() : C_Base(Component::SoundListener)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
    }

private:
};
