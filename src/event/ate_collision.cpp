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

    if(m_start) {
        const Cause& c = get_cause();
        if(c.tile()) {
            const Tile* t = c.get_tile();
            if( (m_other_name == "" || m_other_name == t->get_type()) &&
                (m_my_hitbox == "" || m_my_hitbox == c.my_hitbox()) &&
                (m_other_hitbox == "" || m_other_hitbox == c.other_hitbox())) {
                m_decision = true;
            }
            else {
                m_decision = false;
            }
        }
        else if(c.actor()) {
            const Actor* a = c.get_actor();
            if( (m_other_name == "" || (scope.get_map().get_layer_collection().check_actor(a) && (m_other_name == a->get_name() || m_other_name == a->get_type()))) &&
                (m_my_hitbox == "" || m_my_hitbox == c.my_hitbox()) &&
                (m_other_hitbox == "" || m_other_hitbox == c.other_hitbox())) {
                m_decision = true;
            }
            else {
                m_decision = false;
            }
        }
        else {
            std::cerr << "The collision switch event " << m_name << " wasn't triggered by a collision!\n";
            std::cerr << "This means the event gets used wrongly, fix this error in your events!\n";
            return EventSignal::end;
        }
        m_start = false;
    }

    EventSignal sig;
    if(m_decision) {
        if(m_success.valid()) {sig = m_success->process(scope);}
        else {return EventSignal::end;}
    }
    else {
        if(m_failure.valid()) {sig = m_failure->process(scope);}
        else {return EventSignal::end;}
    }
    if(sig == EventSignal::next) return get_signal();
    else {return sig;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteCollision::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    PropertyParser<AteCollision> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(&AteCollision::m_other_name, "OTHER_NAME");
    parser.add(&AteCollision::m_my_hitbox, "MY_HITBOX");
    parser.add(&AteCollision::m_other_hitbox, "OTHER_HITBOX");
    std::string success_name, failure_name;
    parser.add(success_name, "SUCCESS");
    parser.add(failure_name, "FAILURE");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(success_name != "") {
        if(!base_map.check_event_convert_actor(success_name)) {
            std::cerr << "Event " << success_name << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_success = base_map.get_event_convert_actor(success_name);
        }
    }

    if(failure_name != "") {
        if(!base_map.check_event_convert_actor(failure_name)) {
            std::cerr << "Event " << failure_name << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_failure = base_map.get_event_convert_actor(failure_name);
        }
    }

    return XML_SUCCESS;
}
