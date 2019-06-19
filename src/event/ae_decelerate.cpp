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
#include "event/ae_decelerate.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeDecelerate::m_alias = "AeDecelerate";

const bool AeDecelerate::good = Event<Actor>::register_class<AeDecelerate>();

/**
 * @brief Increase speed of an actor
 * @param actor The actor which moves
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeDecelerate::process(Actor& actor) {
    // Syncs members with possibly linked DataBlock variables
    m_property_listener.listen(actor);

    bool at_zero_speed = false;
    DataBlock& values = actor.get_data();

    float x_factor = m_x_factor * actor.get_map().get_delta_time();
    float y_factor = m_y_factor * actor.get_map().get_delta_time();

    if(x_factor != 0.0f) {
        float temp = values.get_val_float(m_x_speed_name);
        if(temp > 0.0f) {
            if(temp > x_factor) {values.set_val(m_x_speed_name, temp - x_factor);}
            else {values.set_val(m_x_speed_name, 0.0f);}
        }
        else if(temp < 0.0f) {
            if(temp < -x_factor) {values.set_val(m_x_speed_name, temp + x_factor);}
            else {values.set_val(m_x_speed_name, 0.0f);}
        }
        else {at_zero_speed = true;}
    }

    if(y_factor != 0.0f) {
        float temp = values.get_val_float(m_y_speed_name);
        if(temp > 0.0f) {
            if(temp > y_factor) {values.set_val(m_y_speed_name, temp - y_factor);}
            else {values.set_val(m_y_speed_name, 0.0f);}
        }
        else if(temp < 0.0f) {
            if(temp < -y_factor) {values.set_val(m_y_speed_name, temp + y_factor);}
            else {values.set_val(m_y_speed_name, 0.0f);}
        }
        else {at_zero_speed = true;}
    }

    if(at_zero_speed) {return EventSignal::abort;}
    else {return EventSignal::end;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeDecelerate::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map, m_property_listener);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
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
