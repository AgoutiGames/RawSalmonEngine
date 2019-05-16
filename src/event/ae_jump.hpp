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
#ifndef AE_JUMP_HPP_INCLUDED //< Change this
#define AE_JUMP_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Perform a jump
 */
class AeJump : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeJump* create() const override {return new AeJump();}
        AeJump* clone() const override {return new AeJump(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias; //< Define this!
        // vv Add members with default values
        float m_duration = 0.5f;
        float m_jump_height = 400.0f;

        bool m_slow_on_release = true;
        float m_slow_factor = 0.5f;

        AnimationType m_animation = AnimationType::jump;
        Direction m_anim_dir = Direction::current;

        float m_speed = 0;
        float m_deceleration = 0;
        bool m_started = false;
        // Members
};

#endif // AE_JUMP_HPP_INCLUDED
