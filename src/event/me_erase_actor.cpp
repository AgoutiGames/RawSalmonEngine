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
#include "event/me_erase_actor.hpp"

#include <string>
#include <map>
#include <iostream>

#include "map/mapdata.hpp"
#include "event/property_parser.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string MeEraseActor::m_alias = "MeEraseActor";

const bool MeEraseActor::good = Event<MapData>::register_class<MeEraseActor>();

/**
 * @brief Do ...
 * @param MapData The MapData which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal MeEraseActor::process(MapData& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    if(m_actor_pointer != nullptr) {
        if(scope.get_layer_collection().erase_actor(m_actor_pointer)) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    else if(scope.get_layer_collection().erase_actor(m_actor_name)) {
        return EventSignal::end;
    }
    else {
        return EventSignal::abort;
    }
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch MapDatas or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError MeEraseActor::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<MeEraseActor> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(&MeEraseActor::m_actor_name, "ACTOR_NAME");

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
