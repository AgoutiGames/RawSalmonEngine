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
#include "event/ae_move.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeMove::m_alias = "AeMove";

const bool AeMove::good = Event<Actor>::register_class<AeMove>();

/**
 * @brief Increase speed of an actor
 * @param actor The actor which moves
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMove::process(Actor& actor) {

    // constexpr float FPS = 60;
    float delta = actor.get_map().get_delta_time();

    DataBlock& values = actor.get_data();

    bool collided = false;
    if(m_x_factor != 0.0f) {
        float temp = values.get_val_float(m_x_speed_name);
        if(temp != 0.0f) {
            if(!actor.move((temp * m_x_factor) * delta, 0.0f)) {
                values.set_val(m_x_speed_name, 0.0f);
                collided = true;
            }
        }
    }

    if(m_y_factor != 0.0f) {
        float temp = values.get_val_float(m_y_speed_name);
        if(temp != 0.0f) {
            if(!actor.move(0.0f, (temp * m_y_factor) * delta)) {
                values.set_val(m_y_speed_name, 0.0f);
                collided = true;
            }
        }
    }

    if(collided) {return EventSignal::abort;}
    else {return EventSignal::end;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMove::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    parser.add(m_x_factor, "XFACTOR");
    parser.add(m_y_factor, "YFACTOR");
    parser.add(m_x_speed_name, "XSPEED_NAME");
    parser.add(m_y_speed_name, "YSPEED_NAME");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
