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
#include "event/ate_on_ground.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AteOnGround::m_alias = "AteOnGround";

const bool AteOnGround::good = Event<Actor>::register_class<AteOnGround>();

/**
 * @brief Trigger specific event if on ground or not
 * @param actor The actor which is or isn't on ground
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteOnGround::process(Actor& actor) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, actor);

    // Parse and check direction value
    Direction dir = str_to_direction(m_direction_string);
    if(dir == Direction::invalid || dir == Direction::current) {
        Logger(Logger::error) << info() << " Direction: " << m_direction_string << " is an invalid direction";
        return EventSignal::abort;
    }

    if(actor.on_ground(dir, m_tolerance)) {
        return EventSignal::end;
    }
    else {
        return EventSignal::abort;
    }
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteOnGround::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AteOnGround> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");

    // Add additional members here
    parser.add(&AteOnGround::m_direction_string, "DIRECTION");
    parser.add(&AteOnGround::m_tolerance, "TOLERANCE");

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
