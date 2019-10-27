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
#include "event/ae_sound.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeSound::m_alias = "AeSound";

const bool AeSound::good = Event<Actor>::register_class<AeSound>();

/**
 * @brief Play a sound
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSound::process(Actor& actor) {
    (void)actor; // Mute unused var warning :
    m_sound.play();
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSound::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    PropertyParser<AeSound> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    std::string m_sound_path = "";
    parser.add(m_sound_path, "PATH");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(m_sound_path == "") {
        Logger(Logger::error) << "Missing sound path property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    m_sound = SoundEffect(base_map.get_file_path() + m_sound_path);
    if(!m_sound.good()) {
        Logger(Logger::error) << "Audio file: " << base_map.get_file_path() + m_sound_path << " not found!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
