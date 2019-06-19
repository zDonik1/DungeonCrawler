#pragma once

#include <string>

using Frame = unsigned int;

class SpriteSheet;

class Anim_Base
{
    friend class SpriteSheet;

public:
    Anim_Base();
    virtual ~Anim_Base();

    std::string getName();
    unsigned int getFrame() const;
    void setSpriteSheet(SpriteSheet* l_sheet);
    bool setFrame(const unsigned int& l_frame);
    void setLooping(const bool& l_loop);
    void setName(const std::string& l_name);

    bool isInAction();
    bool isPlaying();
    bool checkMoved();

    void play();
    void pause();
    void stop();
    void reset();

    virtual void update(const float& l_dT);

    // overloaded operator which is used in ResourceManager::requireResource() to read all the info about animation
    //   from a file
    friend std::stringstream& operator >>(std::stringstream& l_stream, Anim_Base& anim)
    {
        anim.readIn(l_stream);
        return l_stream;
    }

protected:
    virtual void frameStep() = 0;
    virtual void cropSprite() = 0;
    virtual void readIn(std::stringstream& l_stream) = 0;

    Frame m_currentFrame; // current frame number
    Frame m_frameStart; // starting frame number
    Frame m_frameEnd; // ending frame number
    Frame m_frameRow; // row or "type of animation" number

    // action start and end decided when a specific action takes place during animation
    int m_frameActionStart;
    int m_frameActionEnd;

    float m_frameTime; // time bewteen each frame
    float m_elapsedTime;
    bool m_loop;
    bool m_playing;
    bool m_hasMoved;
    std::string m_name; // animation name
    SpriteSheet* m_spriteSheet; // pointer to spritesheet used;
};
