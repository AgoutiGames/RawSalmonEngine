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
#include "event/ae_multi.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/game_types.hpp"

const std::string AeMulti::m_alias = "AeMulti";

const bool AeMulti::good = Event<Actor>::register_class<AeMulti>();

/**
 * @brief Process the contained events
 * @param actor The actor which gets processed by each event
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMulti::process(Actor& actor) {
    EventSignal sig = m_events.process_events(actor);
    switch(sig) {
    // If all events played without problems
    case (EventSignal::next) : {return get_signal();}
    // If something special happened
    default : {return sig;}
    }
    /*
    if(!m_events.empty()) {
        for(unsigned i = 0; i < m_events.size(); i++) {
            ActorEvent* event = m_events[i];
            EventSignal e_signal = event->process(actor);
            if(e_signal == EventSignal::stop) {
                return signal();
            }
            else if(e_signal == EventSignal::end || e_signal == EventSignal::abort) {
                m_events[i]->kill();
                m_events.erase(m_events.begin() + i);
                i--;
            }
            else if(e_signal == EventSignal::erase) {return e_signal;}
        }
    }
    if(m_events.is_empty()) {
        return EventSignal::end;
    }
    return get_signal();*/
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */ /*
tinyxml2::XMLError AeMulti::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    unsigned current_event = 1;

    while(source != nullptr) {
        const char* p_name;
        const char* p_value;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse additional members

        else if(name == "NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            m_name = std::string(p_value);
        }

        else if(name == "PRIORITY") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            m_priority = str_to_priority(value);
            if(m_priority == Priority::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == "SIGNAL") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            m_signal = str_to_event_signal(value);
            if(m_signal == EventSignal::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == std::to_string(current_event)) {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            if(!base_map.check_event(value)) {
                std::cerr << "Event " << value << " has not been parsed before!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                m_events.add_event(base_map.get_event(value));
                current_event++;
            }
        }

        else {
            std::cerr << "Unknown event property \""<< p_name << "\" specified\n";
            std::cerr << "If the property is a number, there is a number gap between the events\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        source = source->NextSiblingElement("property");
    }
    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }
    return XML_SUCCESS;
}*/

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMulti::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    Parser parser(base_map);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");
    std::vector<std::string> event_names;
    parser.add(event_names);

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    for(std::string value : event_names) {
        if(!base_map.check_event(value)) {
            std::cerr << "Event " << value << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_events.add_event(base_map.get_event(value));
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Set cause to every contained event
 */
void AeMulti::set_cause(Cause x) {
    Event<Actor>::set_cause(x);
    for(Event<Actor>* e : m_events.get_events()) {
        e->set_cause(x);
    }
}

