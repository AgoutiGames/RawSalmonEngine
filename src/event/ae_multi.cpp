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
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AeMulti::m_alias = "AeMulti";

AeMulti::AeMulti(std::vector<ActorEvent*> event_list) :
EventContainer(),
m_events{event_list}
{

}

/**
 * @brief Process the contained events
 * @param actor The actor which gets processed by each event
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMulti::process(Actor& actor) {
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
    if(m_events.empty()) {
        return EventSignal::end;
    }
    return signal();
}

/// Create event and return pointer to it
AeMulti* AeMulti::create(std::vector<ActorEvent*> event_list) {
    AeMulti temp(event_list);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMulti::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    //XMLError eResult;

    // Additional members
    std::string event_name("");
    std::vector<ActorEvent*> event_list;
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

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
            event_name = std::string(p_value);
        }

        else if(name == "PRIORITY") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            prio = str_to_priority(value);
            if(prio == Priority::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == "SIGNAL") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            sig = str_to_event_signal(value);
            if(sig == EventSignal::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == std::to_string(current_event)) {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            if(!map.check_event(value)) {
                std::cerr << "Event " << value << " has not been parsed before!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                event_list.push_back(map.get_event(value));
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
    if(event_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }
    ActorEvent* event = create(event_list);
    event->set_priority(prio);
    event->set_signal(sig);
    event->set_name(event_name);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}

/**
 * @brief When copied, also copy contained events
 */
ActorEvent* AeMulti::copy() const{
    std::vector<ActorEvent*> new_list;
    for(ActorEvent* e : m_events) {
        new_list.push_back(e->copy());
    }
    return create(new_list);
}

/**
 * @brief Set cause to every contained event
 */
void AeMulti::set_cause(Cause x) {
    ActorEvent::set_cause(x);
    for(ActorEvent* e : m_events) {
        e->set_cause(x);
    }
}

/**
 * @brief Set cause to every contained event
 */
void AeMulti::set_key(SDL_Keysym x) {
    ActorEvent::set_key(x);
    for(ActorEvent* e : m_events) {
        e->set_key(x);
    }
}
