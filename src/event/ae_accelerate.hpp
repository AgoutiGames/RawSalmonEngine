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
#ifndef AE_ACCELERATE_HPP_INCLUDED //< Change this
#define AE_ACCELERATE_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Increase speed var of actor while respecting limits
 */
class AeAccelerate : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeAccelerate* create() const override {return new AeAccelerate();}
        AeAccelerate* clone() const override {return new AeAccelerate(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AeAccelerate> m_property_listener;
        // vv Add members with default values
        float m_x_max_speed = 500.0f;
        float m_y_max_speed = 500.0f;
        float m_x_factor = 40.0f;
        float m_y_factor = 40.0f;
        std::string m_x_speed_name = "__XSPEED";
        std::string m_y_speed_name = "__YSPEED";
};

#endif // AE_ACCELERATE_HPP_INCLUDED
