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
#include "event/ae_jump.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "event/ae_fall.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeJump::m_alias = "AeJump";

const bool AeJump::good = Event<Actor>::register_class<AeJump>();

/**
 * @brief Perform a jump
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeJump::process(Actor& actor) {
    if(!m_started) {
        actor.get_event_queue().scrap_event(AeFall().get_type());
        actor.get_event_queue().block_event(AeFall().get_type());
        m_started = true;
    }

    constexpr float FPS = 60;
    float delta = actor.get_map().get_delta_time() * FPS;

    if(m_deceleration == 0) {
        float steps = delta * m_duration;
        steps = m_jump_height / steps;
        m_speed = steps * 2;
        m_deceleration = m_speed / (delta * m_duration);
    }

    if(!actor.move(0, -m_speed)) {
        actor.get_event_queue().unblock_event(AeFall().get_type());
        return EventSignal::abort;
    }

    actor.animate(m_animation, m_anim_dir);

    if(m_slow_on_release) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if ( !(keys[get_cause().get_key().scancode]) ) {
            m_deceleration = m_speed * m_slow_factor + m_deceleration;
            m_slow_on_release = false;
        }
    }

    m_speed -= m_deceleration;

    if(m_speed <= 0) {
        actor.get_event_queue().unblock_event(AeFall().get_type());
        return EventSignal::end;
    }
    else {return get_signal();}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeJump::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(m_duration, "JUMP_DURATION");
    parser.add(m_jump_height, "JUMP_HEIGHT");
    parser.add(m_slow_on_release, "SLOW_ON_RELEASE");
    parser.add(m_slow_factor, "SLOW_FACTOR");
    parser.add(m_animation, "ANIMATION_TYPE");
    parser.add(m_anim_dir, "ANIMATION_DIRECTION");

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
