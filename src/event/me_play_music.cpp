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
#include "event/me_play_music.hpp"

#include <string>
#include <map>
#include <iostream>

#include "map/mapdata.hpp"
#include "event/property_parser.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string MePlayMusic::m_alias = "MePlayMusic";

const bool MePlayMusic::good = Event<MapData>::register_class<MePlayMusic>();

/**
 * @brief Do ...
 * @param MapData The MapData which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal MePlayMusic::process(MapData& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    m_music.play(m_repititions, m_fade_in);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch MapDatas or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError MePlayMusic::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    PropertyParser<MePlayMusic> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    std::string path;
    parser.add(path, "PATH");
    parser.add(&MePlayMusic::m_repititions, "REPITITIONS");
    parser.add(&MePlayMusic::m_fade_in, "FADE IN");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!" << std::endl;
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"" << std::endl;
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    m_music = Music(base_map.get_file_path() + path);
    if(!m_music.good()) {
        Logger(Logger::error) << "Failed parsing music file: " << path << " !" << std::endl;
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
