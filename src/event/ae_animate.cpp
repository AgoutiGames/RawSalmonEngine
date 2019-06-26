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
#include "event/ae_animate.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeAnimate::m_alias = "AeAnimate";

const bool AeAnimate::good = Event<Actor>::register_class<AeAnimate>();

/**
 * @brief Animate the actor
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 * @note Adds up cycles, anim frames and game frames
 */
EventSignal AeAnimate::process(Actor& actor) {
    // fetch only once!
    if(m_first) {
        // Syncs members with possibly linked DataBlock variables
        listen(m_property_listener, *this, actor);
        m_first = false;
    }

    AnimSignal sig = actor.animate_trigger(m_animation, m_direction, m_speed);
    if(sig == AnimSignal::missing) {return EventSignal::abort;}
    if(m_cycles > 0) {
        if(sig == AnimSignal::wrap) {
            m_cycles--;
        }
    }
    else if(m_anim_frames > 0) {
        if(sig != AnimSignal::none) {
            m_anim_frames --;
        }
    }
    else if(m_game_frames > 0) {
        m_game_frames--;
    }
    else {return EventSignal::end;}

    return get_signal();
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeAnimate::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeAnimate> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(m_animation, "ANIMATION_TYPE");
    parser.add(m_direction, "DIRECTION");
    parser.add(&AeAnimate::m_speed, "SPEED");
    parser.add(&AeAnimate::m_cycles, "CYCLES");
    parser.add(&AeAnimate::m_anim_frames, "ANIMATION_FRAMES");
    parser.add(&AeAnimate::m_game_frames, "GAME_FRAMES");

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
