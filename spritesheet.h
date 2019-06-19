#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "texturemanager.h"
#include "anim_directional.h"

using Animations = std::unordered_map<std::string, Anim_Base*>;

class SpriteSheet
{
public:
    SpriteSheet(TextureManager* l_textMgr);
    ~SpriteSheet();

    void cropSprite(const sf::IntRect& l_rect);

    sf::Vector2i getSpriteSize();
    Direction getDirection();
    sf::Vector2f getSpritePosition();
    sf::Vector2f getScale();
    Anim_Base* getCurrentAnim();
    const sf::Vector2f &getSheetPadding() const;
    const sf::Vector2f &getSpriteSpacing() const;
    void setSpriteSize(const sf::Vector2i& l_size);
    void setSpritePosition(const sf::Vector2f& l_pos);
    void setDirection(const Direction& l_dir);
    void setSheetPadding(const sf::Vector2f &l_padding);
    void setSpriteSpacing(const sf::Vector2f &l_spacing);

    bool loadSheet(const std::string& l_file);
    void releaseSheet();

    bool setAnimation(const std::string& l_name, const bool& l_play = false, const bool& l_loop = false);

    void update(const float& l_dT);
    void draw(sf::RenderWindow* l_wind);

private:
    std::string m_texture; // name of texture used
    sf::Sprite m_sprite;
    sf::Vector2i m_spriteSize;
    sf::Vector2f m_spriteScale;
    Direction m_direction;

    sf::Vector2f m_sheetPadding;
    sf::Vector2f m_spriteSpacing;

    std::string m_animType; // format of animation implementation in textures
    Animations m_animations;
    Anim_Base* m_animationCurrent;
    TextureManager* m_textureManager;
};
