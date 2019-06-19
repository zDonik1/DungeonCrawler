#ifndef ANIM_DIRECTIONAL_H
#define ANIM_DIRECTIONAL_H

#include "anim_base.h"
#include "direction.h"

// this is the special type of animation which is the implementation of the format of animation
class Anim_Directional : public Anim_Base
{
protected:
    void frameStep();
    void cropSprite();
    void readIn(std::stringstream& l_stream);
};

#endif // ANIM_DIRECTIONAL_H
\
