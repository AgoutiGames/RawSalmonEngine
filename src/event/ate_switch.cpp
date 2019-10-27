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
#include "event/ate_switch.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AteSwitch::m_alias = "AteSwitch";

const bool AteSwitch::good = Event<Actor>::register_class<AteSwitch>();

/**
 * @brief Trigger one of two possible events based on the outcome of a list of events
 * @param Actor The Actor which is processed
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteSwitch::process(Actor& scope) {

    if(m_start) {
        // Syncs members with possibly linked DataBlock variables
        listen(m_property_listener, *this, scope);

        if(m_and + m_or != 1) {
            Logger(Logger::error) << info() << " switch event can only either be linked by \"and\" or \"or\"";
            //return XML_ERROR_PARSING_ATTRIBUTE;
            return EventSignal::abort;
        }

        MapData& base_map = scope.get_map();
        // Parse the two switch events
        if(m_success_name != "") {
            if(!base_map.check_event_convert_actor(m_success_name)) {
                Logger(Logger::error) << info() << " Event " << m_success_name << " has not been parsed before!";
                //return XML_ERROR_PARSING_ATTRIBUTE;
                return EventSignal::abort;
            }
            else {
                m_success = base_map.get_event_convert_actor(m_success_name);
                // Propagate cause down to switch events
                m_success->set_cause(m_cause);
            }
        }
        if(m_failure_name != "") {
            if(!base_map.check_event_convert_actor(m_failure_name)) {
                Logger(Logger::error) << info() << " Event " << m_failure_name << " has not been parsed before!";
                //return XML_ERROR_PARSING_ATTRIBUTE;
                return EventSignal::abort;
            }
            else {
                m_failure = base_map.get_event_convert_actor(m_failure_name);
                // Propagate cause down to switch events
                m_failure->set_cause(m_cause);
            }
        }

        // Parse all trigger events
        std::vector<SmartEvent<Actor>> triggers;

        DataBlock& actor_data = scope.get_data();
        DataBlock& map_data = base_map.get_data();

        for(std::string name : m_trigger_events) {

            // manually fetch possibly referenced variables of the data blocks
            if(name.front() == '*') {
                name.erase(name.begin());

                if(actor_data.check_val_string(name)) {
                    name = actor_data.get_val_string(name);
                }
                else if(map_data.check_val_string(name)) {
                    name = map_data.get_val_string(name);
                }
                else {
                    Logger(Logger::error) << info() << " The trigger event variable: " << name << " isn't defined!";
                    return EventSignal::abort;
                }
            }

            if(!base_map.check_event_convert_actor(name)) {
                Logger(Logger::error) << info() << " Event " << m_failure_name << " has not been parsed before!";
                //return XML_ERROR_PARSING_ATTRIBUTE;
                return EventSignal::abort;
            }
            else {
                triggers.push_back(base_map.get_event_convert_actor(name));
                // Propagate cause down to trigger events
                triggers.back()->set_cause(m_cause);
            }
        }

        // decide if success or failure
        std::vector<bool> cases;
        for(SmartEvent<Actor>& event : triggers) {
            if(event->process(scope) == EventSignal::end) {
                cases.push_back(true);
            }
            else {
                cases.push_back(false);
            }
        }

        if(m_or) {
            m_decision = false;
            for(bool c : cases) {
                m_decision = m_decision || c;
            }
        }
        else if(m_and) {
            m_decision = true;
            for(bool c : cases) {
                m_decision = m_decision && c;
            }
        }
        else {
            Logger(Logger::error) << "AteSwitch screams in agony!";
            return EventSignal::abort;
        }

        // Make sure that if there is no trigger event at all
        // the false failure event gets processed
        if(cases.empty()) {m_decision = false;}

        m_start = false;
    }

    EventSignal sig;

    // Process event based on decision
    if(m_decision) {
        if(!m_success) {return EventSignal::end;}
        sig = m_success->process(scope);
    }
    else {
        if(!m_failure) {return EventSignal::end;}
        sig = m_failure->process(scope);
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
tinyxml2::XMLError AteSwitch::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AteSwitch> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");
    parser.add(&AteSwitch::m_failure_name, "FAILURE");
    parser.add(&AteSwitch::m_success_name, "SUCCESS");
    parser.add(m_trigger_events);
    parser.add(&AteSwitch::m_and, "AND");
    parser.add(&AteSwitch::m_or, "OR");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(m_trigger_events.empty()) {
        Logger(Logger::error) << "Missing trigger events for switch event!";
        return XML_ERROR_PARSING;
    }

    return XML_SUCCESS;
}
