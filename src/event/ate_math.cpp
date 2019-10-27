/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#include "event/ate_math.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AteMath::m_alias = "AteMath";

const bool AteMath::good = Event<Actor>::register_class<AteMath>();

/**
 * @brief Compare two given values by operands and trigger true or false
 * @param Actor The Actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteMath::process(Actor& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    // Upfront error checking
    if(m_type1 != m_type2) {
        Logger(Logger::error) << info() << " has two comparing values of different types!";
        return EventSignal::abort;
    }
    if(m_equals && m_not_equal) {
        Logger(Logger::error) << info() << " cant compare by equalness AND unequalness!";
        return EventSignal::abort;
    }
    if(m_bigger && m_smaller) {
        Logger(Logger::error) << info() << " cant compare by bigger AND smaller!";
        return EventSignal::abort;
    }
    if(m_not_equal && (m_bigger || m_smaller)) {
        Logger(Logger::error) << info() << " != nullifies effect of < or >";
        return EventSignal::abort;
    }
    if(m_equals + m_bigger + m_smaller + m_not_equal == 0) {
        Logger(Logger::error) << info() << " must specify at least one comparison operator";
        return EventSignal::abort;
    }

    // Discriminate cases by value type
    if(m_type1 == PropertyType::Boolean) {
        if(m_bigger || m_smaller) {
            Logger(Logger::error) << info() << "can't compare boolian via < and >";
            return EventSignal::abort;
        }
        if( (m_equals && (m_bool1 == m_bool2)) || (m_not_equal && (m_bool1 != m_bool2)) ) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    if(m_type1 == PropertyType::Float) {
        // Actually you shouldn't compare floats by equalness but we support
        // those rare cases where you have to nevertheless without error
        if( (m_equals && m_float1 == m_float2) ||
            (m_not_equal && m_float1 != m_float2) ||
            (m_bigger && m_float1 > m_float2) ||
            (m_smaller && m_float1 < m_float2)) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    if(m_type1 == PropertyType::Integer) {
        if( (m_equals && m_int1 == m_int2) ||
            (m_not_equal && m_int1 != m_int2) ||
            (m_bigger && m_int1 > m_int2) ||
            (m_smaller && m_int1 < m_int2)) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    if(m_type1 == PropertyType::String) {
        if( (m_equals && m_string1 == m_string2) ||
            (m_not_equal && m_string1 != m_string2) ||
            (m_bigger && m_string1 > m_string2) ||
            (m_smaller && m_string1 < m_string2)) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    // THIS NEVER HAPPENS, compiler stop complaining
    return EventSignal::abort;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteMath::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AteMath> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");

    // Add additional members here
    parser.add(&AteMath::m_equals, "==");
    parser.add(&AteMath::m_bigger, ">");
    parser.add(&AteMath::m_smaller, "<");
    parser.add(&AteMath::m_not_equal, "!=");
    parser.add(&AteMath::m_type1, &AteMath::m_bool1, &AteMath::m_int1, &AteMath::m_float1, &AteMath::m_string1, "VAL1");
    parser.add(&AteMath::m_type2, &AteMath::m_bool2, &AteMath::m_int2, &AteMath::m_float2, &AteMath::m_string2, "VAL2");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
