#include "anim_base.h"
#include "spritesheet.h"

Anim_Base::Anim_Base()
    : m_currentFrame(0)
    , m_frameStart(0)
    , m_frameEnd(0)
    , m_frameRow(0)
    , m_frameActionStart(-1)
    , m_frameActionEnd(-1)
    , m_frameTime(0.f)
    , m_elapsedTime(0.f)
    , m_loop(false)
    , m_playing(false)
    , m_hasMoved(false)
{
}

Anim_Base::~Anim_Base()
{
}

void Anim_Base::setSpriteSheet(SpriteSheet* l_sheet)
{
    m_spriteSheet = l_sheet;
}

bool Anim_Base::setFrame(const unsigned int& l_frame)
{
    if ((l_frame >= m_frameStart && l_frame <= m_frameEnd) || (l_frame >= m_frameEnd && l_frame <= m_frameStart)) {
        m_currentFrame = l_frame;
        m_hasMoved = true;
        return true;
    }

    return false;
}

void Anim_Base::setLooping(const bool& l_loop)
{
    m_loop = l_loop;
}

void Anim_Base::setName(const std::string& l_name)
{
    m_name = l_name;
}

std::string Anim_Base::getName()
{
    return m_name;
}

unsigned int Anim_Base::getFrame() const
{
    return m_currentFrame;
}

// returns true if the frame is in action, which is when the action of the animation is happening
bool Anim_Base::isInAction()
{
    if (m_frameActionStart == -1 || m_frameActionEnd == -1)
        return true;

    return (m_currentFrame >= static_cast<unsigned int>(m_frameActionStart)
            && m_currentFrame <= static_cast<unsigned int>(m_frameActionEnd));
}

bool Anim_Base::isPlaying()
{
    return m_playing;
}

bool Anim_Base::checkMoved()
{
    bool result = m_hasMoved;
    m_hasMoved = false;
    return result;
}

void Anim_Base::play()
{
    m_playing = true;
}

void Anim_Base::pause()
{
    m_playing = false;
}

void Anim_Base::stop()
{
    m_playing = false;
    reset();
}

void Anim_Base::reset()
{
    m_currentFrame = m_frameStart;
    m_elapsedTime = 0.0f;
    cropSprite();
}

// updates the animation by checking if the animation has to be played and going to the next frame
//   this is the method where the flags for playing and looping are checked
void Anim_Base::update(const float& l_dT)
{
    if (!m_playing)
        return;

    m_elapsedTime += l_dT;
    if (m_elapsedTime < m_frameTime)
        return;

    frameStep();
    cropSprite();
    m_elapsedTime = 0;
}
