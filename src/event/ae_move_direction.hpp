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
#ifndef AE_MOVE_DIRECTION_HPP_INCLUDED
#define AE_MOVE_DIRECTION_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Move the actor to the supplied direction for x frames
 */
class AeMoveDirection : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeMoveDirection* create() const override {return new AeMoveDirection();}
        AeMoveDirection* clone() const override {return new AeMoveDirection(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias; //< Define this!
        // vv Add members with default values
        Direction m_direction = Direction::up;
        unsigned m_duration = 1; ///< How many frames the movement persists
        AnimationType m_animation = AnimationType::walk;
};

#endif // AE_MOVE_DIRECTION_HPP_INCLUDED
