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
#include "event/me_ge_wrapper.hpp"

#include <string>
#include <map>
#include <iostream>

#include "core/gameinfo.hpp"
#include "map/mapdata.hpp"
#include "event/property_parser.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string MeGeWrapper::m_alias = "MeGeWrapper";

const bool MeGeWrapper::good = Event<MapData>::register_class<MeGeWrapper>();

/**
 * @brief Do ...
 * @param MapData The MapData which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal MeGeWrapper::process(MapData& scope) {
    scope.get_game().get_event_queue().add_event(m_event);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch MapDatas or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError MeGeWrapper::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    PropertyParser<MeGeWrapper> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    std::string event_name = "";
    parser.add(event_name, "EVENT");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(event_name != "") {
        if(!base_map.check_event<GameInfo>(event_name)) {
            Logger(Logger::error) << "Event " << event_name << " has not been parsed before!";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_event = base_map.get_event<GameInfo>(event_name);
        }
    }

    return XML_SUCCESS;
}
