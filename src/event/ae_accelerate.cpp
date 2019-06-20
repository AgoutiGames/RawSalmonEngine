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
#include "event/ae_accelerate.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeAccelerate::m_alias = "AeAccelerate";

const bool AeAccelerate::good = Event<Actor>::register_class<AeAccelerate>();

/**
 * @brief Increase speed of an actor
 * @param actor The actor which moves
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeAccelerate::process(Actor& actor) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, actor);

    bool hit_limit = false;
    DataBlock& values = actor.get_data();

    float x_factor = m_x_factor * actor.get_map().get_delta_time();
    float y_factor = m_y_factor * actor.get_map().get_delta_time();

    if(x_factor != 0.0f) {

        float temp = values.get_val_float(m_x_speed_name);

        if(m_x_max_speed < 0.0f) {
            if(temp + x_factor < m_x_max_speed) {
                values.set_val(m_x_speed_name, m_x_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_x_speed_name, temp + x_factor);
            }
        }

        else {
            if(temp + x_factor > m_x_max_speed) {
                values.set_val(m_x_speed_name, m_x_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_x_speed_name, temp + x_factor);
            }
        }
    }

    if(y_factor != 0.0f) {
        float temp = values.get_val_float(m_y_speed_name);

        if(m_y_max_speed < 0.0f) {
            if(temp + y_factor < m_y_max_speed) {
                values.set_val(m_y_speed_name, m_y_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_y_speed_name, temp + y_factor);
            }
        }

        else {
            if(temp + y_factor > m_y_max_speed) {
                values.set_val(m_y_speed_name, m_y_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_y_speed_name, temp + y_factor);
            }
        }
    }
    if(hit_limit) {return EventSignal::abort;}
    else{return EventSignal::end;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeAccelerate::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeAccelerate> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(&AeAccelerate::m_x_max_speed, "XMAX_SPEED");
    parser.add(&AeAccelerate::m_y_max_speed, "YMAX_SPEED");
    parser.add(&AeAccelerate::m_x_factor, "XFACTOR");
    parser.add(&AeAccelerate::m_y_factor, "YFACTOR");
    parser.add(&AeAccelerate::m_x_speed_name, "XSPEED_NAME");
    parser.add(&AeAccelerate::m_y_speed_name, "YSPEED_NAME");

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
