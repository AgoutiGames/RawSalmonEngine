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
#include "event/ate_collision.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AteCollision::m_alias = "AteCollision";

const bool AteCollision::good = Event<Actor>::register_class<AteCollision>();

/**
 * @brief Only process the contained event if it matches the collision signature
 * @param Actor The Actor which is processed
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteCollision::process(Actor& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);
    const Collision& c = get_collision();
    if(c.tile()) {
        const Tile* t = c.get_tile();
        if( (m_other_name == "" || m_other_name == t->get_type()) &&
            (m_my_hitbox == "" || m_my_hitbox == c.my_hitbox()) &&
            (m_other_hitbox == "" || m_other_hitbox == c.other_hitbox())) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    else if(c.actor()) {
        const Actor* a = c.get_actor();
        if( (m_other_name == "" || (scope.get_map().get_layer_collection().check_actor(a) && (m_other_name == a->get_name() || m_other_name == a->get_type()))) &&
            (m_my_hitbox == "" || m_my_hitbox == c.my_hitbox()) &&
            (m_other_hitbox == "" || m_other_hitbox == c.other_hitbox())) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    else if(c.mouse()) {
        if(m_my_hitbox == "" || m_my_hitbox == c.my_hitbox()) {
            return EventSignal::end;
        }
        else {
            return EventSignal::abort;
        }
    }
    else {
        Logger(Logger::error) << info() << " wasn't triggered by a collision! "<< "This means the event gets used wrongly, fix this error in your events!";
        return EventSignal::abort;
    }
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteCollision::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AteCollision> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");

    // Add additional members here
    parser.add(&AteCollision::m_other_name, "OTHER_NAME");
    parser.add(&AteCollision::m_my_hitbox, "MY_HITBOX");
    parser.add(&AteCollision::m_other_hitbox, "OTHER_HITBOX");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
