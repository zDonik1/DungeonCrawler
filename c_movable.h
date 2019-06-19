#pragma once

#include <SFML/Graphics.hpp>
#include "c_base.h"
#include "direction.h"

class C_Movable : public C_Base
{
public:
    C_Movable()
        : C_Base(Component::Movable)
        , m_velocityMax(0.f)
        , m_direction((Direction)0)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
        l_stream >> m_velocityMax >> m_speed.x >> m_speed.y;

        unsigned int dir = 0;
        l_stream >> dir;
        m_direction = (Direction)dir;
    }

    void addVelocity(const sf::Vector2f &l_vec)
    {
        m_velocity += l_vec;
        if (std::abs(m_velocity.x) > m_velocityMax) {
            m_velocity.x = m_velocityMax * (m_velocity.x / std::abs(m_velocity.x));
        }

        if (std::abs(m_velocity.y) > m_velocityMax) {
            m_velocity.y = m_velocityMax * (m_velocity.y / std::abs(m_velocity.y));
        }
    }

    void applyFriction(const sf::Vector2f &l_vec)
    {
        if (m_velocity.x != 0 && l_vec.x != 0) {
            if (std::abs(m_velocity.x) - std::abs(l_vec.x) < 0) {
                m_velocity.x = 0;
            }
            else {
                m_velocity.x += (m_velocity.x > 0 ? l_vec.x * -1 : l_vec.x);
            }
        }

        if (m_velocity.y != 0 && l_vec.y != 0) {
            if (std::abs(m_velocity.y) - std::abs(l_vec.y) < 0) {
                m_velocity.y = 0;
            }
            else {
                m_velocity.y += (m_velocity.y > 0 ? l_vec.y * -1 : l_vec.y);
            }
        }
    }

    void accelerate(const sf::Vector2f &l_vec)
    {
        m_acceleration += l_vec;
    }

    void accelerate(float l_x, float l_y)
    {
        m_acceleration += sf::Vector2f(l_x, l_y);
    }

    void move(const Direction &l_dir)
    {
        if (l_dir == Direction::Up) {
            m_acceleration.y -= m_speed.y;
        }
        else if (l_dir == Direction::Down) {
            m_acceleration.y += m_speed.y;
        }
        else if (l_dir == Direction::Left) {
            m_acceleration.x -= m_speed.x;
        }
        else if (l_dir == Direction::Right) {
            m_acceleration.x += m_speed.x;
        }
    }

    const sf::Vector2f &getVelocity() const
    {
        return m_velocity;
    }

    float getMaxVelocity() const
    {
        return m_velocityMax;
    }

    const sf::Vector2f &getSpeed() const
    {
        return m_speed;
    }

    const sf::Vector2f &getAcceleration() const
    {
        return m_acceleration;
    }

    const Direction &getDirection() const
    {
        return m_direction;
    }

    void setAcceleration(const sf::Vector2f &l_vec)
    {
        m_acceleration = l_vec;
    }

    void setVelocity(const sf::Vector2f &l_vec)
    {
        m_velocity = l_vec;
    }

    void setDirection(const Direction &l_dir)
    {
        m_direction = l_dir;
    }

private:
    sf::Vector2f m_velocity;
    float m_velocityMax;
    sf::Vector2f m_speed;
    sf::Vector2f m_acceleration;
    Direction m_direction;
};
