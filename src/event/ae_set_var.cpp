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
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeSetVar::m_alias = "AeSetVar";

const bool AeSetVar::good = Event<Actor>::register_class<AeSetVar>();

/**
 * @brief Do ...
 * @param actor The actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSetVar::process(Actor& actor) {
    DataBlock& data = actor.get_data();
    switch(m_value.type) {

        case Value::Bool : {
            data.set_val(m_val_name, m_value.b);
        }
        break;

        case Value::Int : {
            if(m_add) {
                int temp = data.get_val_int(m_val_name);
                data.set_val(m_val_name, temp + m_value.i);
            }
            else if(m_mult) {
                int temp = data.get_val_int(m_val_name);
                data.set_val(m_val_name, temp * m_value.i);
            }
            else {data.set_val(m_val_name, m_value.i);}
        }
        break;

        case Value::Float : {
            if(m_add) {
                float temp = data.get_val_float(m_val_name);
                data.set_val(m_val_name, temp + m_value.f);
            }
            else if(m_mult) {
                float temp = data.get_val_float(m_val_name);
                data.set_val(m_val_name, temp * m_value.f);
            }
            else {data.set_val(m_val_name, m_value.f);}
        }
        break;

        case Value::String : {
            if(m_add) {
                std::string temp = data.get_val_string(m_val_name);
                data.set_val(m_val_name, temp + m_value.s);
            }
            else {data.set_val(m_val_name, m_value.s);}
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
    using namespace tinyxml2;

    Parser parser(base_map);

    std::string str_type;
    parser.add(str_type, "TYPE");

    XMLError eResult = parser.parse_ignore_unknown(source);
    if(eResult != XML_SUCCESS && eResult != XML_NO_ATTRIBUTE) {
        std::cerr << "Failed at parsing value type!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(str_type == "BOOL") {
        m_value.type = Value::Bool;
    }
    else if(str_type == "INT") {
        m_value.type = Value::Int;
    }
    else if(str_type == "FLOAT") {
        m_value.type = Value::Float;
    }
    else if(str_type == "STRING") {
        m_value.type = Value::String;
    }
    else {
        // Do nothing, default value is set to FLOAT
    }

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    parser.add(m_add, "+=");
    parser.add(m_mult, "*=");
    parser.add(m_val_name, "VAL_NAME");

    const std::string tag = "VALUE";
    switch(m_value.type) {
    case(Value::Bool) : {parser.add(m_value.b, tag); break;}
    case(Value::Int) : {parser.add(m_value.i, tag); break;}
    case(Value::Float) : {parser.add(m_value.f, tag); break;}
    case(Value::String) : {parser.add(m_value.s, tag); break;}
    }

    eResult = parser.parse(source);

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

    if(m_value.type == Value::Bool && (m_add || m_mult)) {
        std::cerr << "ae_set_var of type bool can't handle adding or multiplying instruction!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
