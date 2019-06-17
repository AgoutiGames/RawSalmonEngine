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
#include "event/ate_single.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AteSingle::m_alias = "AteSingle";

const bool AteSingle::good = Event<Actor>::register_class<AteSingle>();

/**
 * @brief Do ...
 * @param Actor The Actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteSingle::process(Actor& scope) {
    if(m_first_execution) {
        scope.get_event_queue().block_event(m_name);
        m_first_execution = false;
    }
    EventSignal sig = m_event->process(scope);
    if(sig == EventSignal::abort || sig == EventSignal::end) {
        scope.get_event_queue().unblock_event(m_name);
    }
    return sig;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteSingle::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    std::string event_name;
    parser.add(event_name, "EVENT_NAME");

    // Add additional members here
    //parser.add(m_STUFF, "STUFF");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(event_name == "") {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        std::cerr << "Missing EVENT_NAME property!\n";
        return XML_ERROR_PARSING;
    }

    if(!base_map.check_event_convert_actor(event_name)) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        std::cerr << "There is no event called: " << event_name << " that can be triggered!\n";
        return XML_ERROR_PARSING;
    }
    m_event = base_map.get_event_convert_actor(event_name);

    return XML_SUCCESS;
}