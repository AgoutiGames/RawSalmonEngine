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
#ifndef AE_MOVE_HPP_INCLUDED //< Change this
#define AE_MOVE_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Move Actor by its current speed
 */
class AeMove : public Event<Actor>{
    // The default interface block (copy this!)
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        Event<Actor>* create(tinyxml2::XMLElement* source, MapData& base_map) const override;
        EventSignal process(Actor& actor) override;
        ~AeMove() override {}

        std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}

    private:
        static bool good;
        static std::string m_alias; //< Define this!
        // Members
        float m_x_factor = 1.0f;
        float m_y_factor = 1.0f;
        std::string m_x_speed_name = "__XSPEED";
        std::string m_y_speed_name = "__YSPEED";
};

#endif // AE_MOVE_HPP_INCLUDED
