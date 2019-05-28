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
#include "event/ate_on_ground.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AteOnGround::m_alias = "AteOnGround";

const bool AteOnGround::good = Event<Actor>::register_class<AteOnGround>();

/**
 * @brief Trigger specific event if on ground or not
 * @param actor The actor which is or isn't on ground
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteOnGround::process(Actor& actor) {
    EventSignal sig;
    if(m_start) {
        if(!m_continuous) {
            m_decision = actor.on_ground(m_direction, m_tolerance);
            if(m_decision) {
                std::cerr << "On ground!\n";
            }
            else {
                std::cerr << "NOT on ground!\n";
            }
        }
        m_start = false;
    }

    if(m_continuous) {
        m_decision = actor.on_ground(m_direction, m_tolerance);
    }

    if(m_decision) {
        if(m_success == nullptr) {return EventSignal::end;}
        sig = m_success->process(actor);
    }
    else {
        if(m_failure == nullptr) {return EventSignal::end;}
        sig = m_failure->process(actor);
    }

    if(sig == EventSignal::next) return get_signal();
    else {return sig;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteOnGround::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(m_direction, "DIRECTION");
    parser.add(m_continuous, "CONTINUOUS");
    parser.add(m_tolerance, "TOLERANCE");
    std::string sucess_name = "";
    parser.add(sucess_name, "SUCCESS");
    std::string failure_name = "";
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

    if(sucess_name != "") {
        if(!base_map.check_event(sucess_name)) {
            std::cerr << "Event " << sucess_name << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_success = std::unique_ptr<Event<Actor>>(base_map.get_event(sucess_name));
        }
    }

    if(failure_name != "") {
        if(!base_map.check_event(failure_name)) {
            std::cerr << "Event " << failure_name << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_failure = std::unique_ptr<Event<Actor>>(base_map.get_event(failure_name));
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Set cause to every contained event
 */
void AteOnGround::set_cause(Cause x) {
    Event<Actor>::set_cause(x);
    if(m_success != nullptr) m_success->set_cause(x);
    if(m_failure != nullptr) m_failure->set_cause(x);
}
