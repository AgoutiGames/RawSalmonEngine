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
#include "event/ae_move_direction.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeMoveDirection::m_alias = "AeMoveDirection";

const bool AeMoveDirection::good = Event<Actor>::register_class<AeMoveDirection>();

/**
 * @brief Move the actor to the supplied direction for x frames
 * @param actor The actor which should move
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMoveDirection::process(Actor& actor) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, actor);

    // process stuff
    float speed = m_speed * actor.get_map().get_delta_time();
    if (m_duration != 0) {
        std::vector<float> mov_factors;
        if(m_direction == Direction::current) {mov_factors = dir_to_mov(actor.get_direction());}
        else {mov_factors = dir_to_mov(m_direction);}
        if(actor.move(mov_factors[0] * speed, mov_factors[1] * speed)) {
            actor.animate(m_animation, m_direction);
            m_duration--;
        }
        else {return EventSignal::abort;}
    }
    if (m_duration == 0) return EventSignal::end;
    else return get_signal();
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMoveDirection::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeMoveDirection> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(&AeMoveDirection::m_duration, "DURATION");
    parser.add(m_direction, "DIRECTION");
    parser.add(m_animation, "ANIMATION_TYPE");
    parser.add(&AeMoveDirection::m_speed, "SPEED");

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
