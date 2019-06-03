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
#include "event/ae_me_wrapper.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeMeWrapper::m_alias = "AeMeWrapper";

const bool AeMeWrapper::good = Event<Actor>::register_class<AeMeWrapper>();

/**
 * @brief Do ...
 * @param Actor The Actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMeWrapper::process(Actor& scope) {
    scope.get_map().get_event_queue().add_event(m_event);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMeWrapper::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    std::string event_name = "";
    parser.add(event_name, "EVENT");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(event_name != "") {
        if(!base_map.check_event<MapData>(event_name)) {
            std::cerr << "Event " << event_name << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_event = base_map.get_event<MapData>(event_name);
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Set cause to every contained event
 */
void AeMeWrapper::set_cause(Cause x) {
    Event<Actor>::set_cause(x);
    if(m_event) m_event->set_cause(x);
}
