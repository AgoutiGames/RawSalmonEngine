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
#include "event/ae_fall.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeFall::m_alias = "AeFall";

const bool AeFall::good = Event<Actor>::register_class<AeFall>();
/**
 * @brief Let the actor fall
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeFall::process(Actor& actor) {
    constexpr float FPS = 60;
    m_speed += m_acceleration / FPS;
    if(m_speed > m_max_velocity) {m_speed = m_max_velocity;}
    std::vector<float> way = dir_to_mov(m_fall_dir);
    float pos = actor.get_y();

    if(!actor.on_ground(m_fall_dir)) {
        actor.move(way[0] * m_speed / FPS, way[1] * m_speed / FPS, true);
        actor.get_event_queue().block_event(m_alias);
        actor.animate(m_animation, m_anim_dir);
        // std::cerr << "Fall: " << actor.get_y() - pos << " Actor pos x: "<< actor.get_x() << " y: " << actor.get_y() << "\n";
        m_height += std::abs(actor.get_y() - pos);
        return get_signal();
    }
    else {
        actor.get_event_queue().unblock_event(m_alias);
        if(m_death_height != 0 && m_height >= m_death_height) {actor.respond(Response::on_death);}
        return EventSignal::end;
    }
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeFall::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(m_acceleration, "ACCELERATION");
    parser.add(m_max_velocity, "MAX_VELOCITY");
    parser.add(m_death_height, "DEATH_HEIGHT");
    parser.add(m_fall_dir, "FALL_DIRECTION");
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
