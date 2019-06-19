#include "spritesheet.h"

SpriteSheet::SpriteSheet(TextureManager* l_textMgr) :
    m_spriteScale(1.f, 1.f),
    m_direction(Direction::Right),
    m_animationCurrent(nullptr),
    m_textureManager(l_textMgr) {}

SpriteSheet::~SpriteSheet()
{
    releaseSheet();
}

// loads and stores all the information about spritesheet and animations from the file given as parameter
bool SpriteSheet::loadSheet(const std::string& l_file)
{
    std::ifstream sheet;
    sheet.open(Utils::getWorkingDirectory() + l_file);
    if (!sheet.is_open()) {
        std::cout << "! Failed to load file: " << l_file << std::endl;
        return false;
    }

    std::cout << "loading " << l_file << std::endl;
    if (sheet.is_open()) {
        releaseSheet();
        std::string line;
        while (std::getline(sheet, line)) { // this loop goes through each line and loads animation information
            if (line[0] == '|')
                continue;

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;

            if (type == "Texture") {
                if (m_texture != "") {
                    std::cerr << "! Duplicate texture entries in: " << l_file << std::endl;
                    continue;
                }

                std::string texture;
                keystream >> texture;
                if (!m_textureManager->requireResource(texture)) {
                    std::cerr << "! Could not set up the texture: " << texture << std::endl;
                    continue;
                }

                m_texture = texture;
                m_sprite.setTexture(*m_textureManager->getResource(m_texture));
            }
            else if (type == "Size") {
                keystream >> m_spriteSize.x >> m_spriteSize.y;
                setSpriteSize(m_spriteSize);
            }
            else if (type == "Scale") {
                keystream >> m_spriteScale.x >> m_spriteScale.y;
                m_sprite.setScale(m_spriteScale);
            }
            else if (type == "Padding") {
                keystream >> m_sheetPadding.x >> m_sheetPadding.y;
            }
            else if (type == "Spacing") {
                keystream >> m_spriteSpacing.x >> m_spriteSpacing.y;
            }
            else if (type == "AnimationType") {
                keystream >> m_animType;
            }
            else if (type == "Animation") {
                std::string name;
                keystream >> name;
                if (m_animations.find(name) != m_animations.end()) {
                    std::cerr << "! Duplicate animation(" << name << ") in: " << l_file << std::endl;
                    continue;
                }

                Anim_Base* anim = nullptr;
                // a factory is optimal to be used here instead of if/else statements
                if (m_animType == "Directional") {
                    anim = new Anim_Directional;
                }
                else {
                    std::cerr << "! Unknown animation type: " << m_animType << std::endl;
                    continue;
                }

                keystream >> *anim; // calls overloaded operator >> to stream in all the information from .sheet file
                anim->setSpriteSheet(this);
                anim->setName(name);
                anim->reset();
                m_animations.emplace(name, anim); // the animation gets emplaced and stored in the map

                if (m_animationCurrent)
                    continue;

                m_animationCurrent = anim;
                m_animationCurrent->play(); // i have no idea what is this doing here
            }
        }

        sheet.close();
        return true;
    }

    std::cerr << "! Failed loading spritesheet: " << l_file << std::endl;
    return false;
}

// releases the texture used for sprite animations and then deletes all animations, erasing all the entries from map
void SpriteSheet::releaseSheet()
{
    m_textureManager->releaseResource(m_texture);
    m_animationCurrent = nullptr;

    while (m_animations.begin() != m_animations.end()) {
        delete m_animations.begin()->second;
        m_animations.erase(m_animations.begin());
    }
}

Anim_Base *SpriteSheet::getCurrentAnim()
{
    return m_animationCurrent;
}

const sf::Vector2f &SpriteSheet::getSheetPadding() const
{
    return m_sheetPadding;
}

const sf::Vector2f &SpriteSheet::getSpriteSpacing() const
{
    return m_spriteSpacing;
}

sf::Vector2i SpriteSheet::getSpriteSize()
{
    return m_spriteSize;
}

Direction SpriteSheet::getDirection()
{
    return m_direction;
}

sf::Vector2f SpriteSheet::getSpritePosition()
{
    return m_sprite.getPosition();
}

sf::Vector2f SpriteSheet::getScale()
{
    return m_spriteScale;
}

void SpriteSheet::setSpriteSize(const sf::Vector2i& l_size)
{
    m_spriteSize = l_size;
    m_sprite.setOrigin(m_spriteSize.x / 2, m_spriteSize.y);
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& l_pos)
{
    m_sprite.setPosition(l_pos);
}

void SpriteSheet::setDirection(const Direction& l_dir)
{
    if (l_dir == m_direction)
        return;

    m_direction = l_dir;
    m_animationCurrent->cropSprite();
}

void SpriteSheet::setSheetPadding(const sf::Vector2f &l_padding)
{
    m_sheetPadding = l_padding;
}

void SpriteSheet::setSpriteSpacing(const sf::Vector2f &l_spacing)
{
    m_spriteSpacing = l_spacing;
}

// the rectangle given as parameter is used to get the certain part of the texture to be rendered
void SpriteSheet::cropSprite(const sf::IntRect& l_rect)
{
    m_sprite.setTextureRect(l_rect);
}

// finds animation (Anim_Base) using handle and sets it as current animation
bool SpriteSheet::setAnimation(const std::string& l_name, const bool& l_play, const bool& l_loop)
{
    auto itr = m_animations.find(l_name);
    if (itr == m_animations.end())
        return false;

    if (itr->second == m_animationCurrent)
        return false;

    if (m_animationCurrent)
        m_animationCurrent->stop();

    m_animationCurrent = itr->second;
    m_animationCurrent->setLooping(l_loop);

    if (l_play)
        m_animationCurrent->play();

    m_animationCurrent->cropSprite();
    return true;
}

void SpriteSheet::update(const float& l_dT)
{
    m_animationCurrent->update(l_dT);
}

void SpriteSheet::draw(sf::RenderWindow* l_wind)
{
    l_wind->draw(m_sprite);
}
