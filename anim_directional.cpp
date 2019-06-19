#include "anim_directional.h"
#include "spritesheet.h"

// depending on current frame and frame row ("type of animation") a rectangle is calculated and used to select a certain
//   part of the texture (spritesheet)
void Anim_Directional::cropSprite()
{
    sf::Vector2f padding = m_spriteSheet->getSheetPadding();
    sf::Vector2f spacing = m_spriteSheet->getSpriteSpacing();

    sf::IntRect rect((m_spriteSheet->getSpriteSize().x * m_currentFrame) + padding.x + (spacing.x * m_currentFrame),
                     m_spriteSheet->getSpriteSize().y * (m_frameRow + (short)m_spriteSheet->getDirection()) + padding.y +
                        ((m_frameRow + (short)m_spriteSheet->getDirection()) * spacing.y),
                     m_spriteSheet->getSpriteSize().x,
                     m_spriteSheet->getSpriteSize().y);

    m_spriteSheet->cropSprite(rect);
}

// goes to the next frame (depending on animation being played forwards or backwards)
void Anim_Directional::frameStep()
{
    bool b = setFrame(m_currentFrame + (m_frameStart <= m_frameEnd ? 1 : -1));
    if (b) {
        return;
    }

    if (m_loop) {
        setFrame(m_frameStart);
    }
    else {
        setFrame(m_frameEnd);
        pause();
    }
}

// used in overloaded operator >> to read all the information from a file
void Anim_Directional::readIn(std::stringstream& l_stream)
{
    l_stream >> m_frameStart >> m_frameEnd >> m_frameRow >> m_frameTime >> m_frameActionStart >> m_frameActionEnd;
}
