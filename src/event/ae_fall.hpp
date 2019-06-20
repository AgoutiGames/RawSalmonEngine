/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
 *
 * This file is part of the RawSalmonEngine.
 *
 * The RawSalmonEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The RawSalmonEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the RawSalmonEngine.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AE_FALL_HPP_INCLUDED //< Change this
#define AE_FALL_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Let the actor fall
 */
class AeFall : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeFall* create() const override {return new AeFall();}
        AeFall* clone() const override {return new AeFall(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AeFall> m_property_listener;
        // vv Add members with default values
        float m_acceleration = 500.0f;
        float m_max_velocity = 1000.0f;
        int m_death_height = 0.0f;
        Direction m_fall_dir = Direction::down;

        AnimationType m_animation = AnimationType::fall;
        Direction m_anim_dir = Direction::current;

        float m_speed = 0;
        float m_height = 0;
};

#endif // AE_FALL_HPP_INCLUDED
