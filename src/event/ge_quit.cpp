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
#include "event/ge_quit.hpp"

#include <string>
#include <map>
#include <iostream>
#include <SDL2/SDL.h>

#include "core/gameinfo.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string GeQuit::m_alias = "GeQuit";

const bool GeQuit::good = Event<GameInfo>::register_class<GeQuit>();

/**
 * @brief Do ...
 * @param GameInfo The GameInfo which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal GeQuit::process(GameInfo& scope) {
    // Mute unused parameter warning
    (void)scope;
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    sdlevent.quit.timestamp = SDL_GetTicks();
    SDL_PushEvent(&sdlevent);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch GameInfos or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError GeQuit::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<GeQuit> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

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
