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
#include "event/ae_set_var.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeSetVar::m_alias = "AeSetVar";

const bool AeSetVar::good = Event<Actor>::register_class<AeSetVar>();

/**
 * @brief Do ...
 * @param actor The actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSetVar::process(Actor& actor) {

    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, actor);

    if(m_type == PropertyType::Boolean && (m_add || m_mult)) {
        std::cerr << "ae_set_var of type bool can't handle adding or multiplying instruction!\n";
        return EventSignal::abort;
    }

    DataBlock& data = actor.get_data();
    switch(m_type) {

        case PropertyType::Boolean : {
            data.set_val(m_val_name, m_bool);
        }
        break;

        case PropertyType::Integer : {
            if(m_add) {
                int temp = data.get_val_int(m_val_name);
                data.set_val(m_val_name, temp + m_int);
            }
            else if(m_mult) {
                int temp = data.get_val_int(m_val_name);
                data.set_val(m_val_name, temp * m_int);
            }
            else {data.set_val(m_val_name, m_int);}
        }
        break;

        case PropertyType::Float : {
            if(m_add) {
                float temp = data.get_val_float(m_val_name);
                data.set_val(m_val_name, temp + m_float);
            }
            else if(m_mult) {
                float temp = data.get_val_float(m_val_name);
                data.set_val(m_val_name, temp * m_float);
            }
            else {data.set_val(m_val_name, m_float);}
        }
        break;

        case PropertyType::String : {
            if(m_add) {
                std::string temp = data.get_val_string(m_val_name);
                data.set_val(m_val_name, temp + m_string);
            }
            else {data.set_val(m_val_name, m_string);}
        }
        break;
    }
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSetVar::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeSetVar> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    parser.add(m_add, "+=");
    parser.add(m_mult, "*=");
    parser.add(m_val_name, "VAL_NAME");
    parser.add(&AeSetVar::m_type, &AeSetVar::m_bool, &AeSetVar::m_int, &AeSetVar::m_float, &AeSetVar::m_string, "VALUE");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(m_val_name == "") {
        std::cerr << "Missing value name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
