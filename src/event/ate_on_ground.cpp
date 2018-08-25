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
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AteOnGround::m_alias = "AteOnGround";

AteOnGround::AteOnGround(ActorEvent* success, ActorEvent* failure, Direction direction, bool continuous, int tolerance) :
EventContainer(),
m_success{success},
m_failure{failure},
m_direction{direction},
m_continuous{continuous},
m_tolerance{tolerance}
{

}

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
        }
        m_start = false;
    }

    if(m_continuous) {
        m_decision = actor.on_ground(m_direction, m_tolerance);
    }

    if(m_decision) {
        if(m_success == nullptr) return EventSignal::end;
        sig = m_success->process(actor);
    }
    else {
        if(m_failure == nullptr) return EventSignal::end;
        sig = m_failure->process(actor);
    }

    if(sig == EventSignal::abort) return sig;
    else if(sig == EventSignal::end) return sig;
    else if(sig == EventSignal::erase) return sig;
    else return signal();
}

/// Create event and return pointer to it
AteOnGround* AteOnGround::create(ActorEvent* success, ActorEvent* failure, Direction direction, bool continuous, int tolerance) {
    AteOnGround temp(success, failure, direction, continuous, tolerance);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteOnGround::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    XMLError eResult;

    // Additional members
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

    ActorEvent* success = nullptr;
    ActorEvent* failure = nullptr;
    Direction direction = Direction::down;
    bool continuous = false;
    int tolerance = 0;

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

        else if(name == "SUCCESS") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            if(!map.check_event(value)) {
                std::cerr << "Event " << value << " has not been parsed before!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                success = map.get_event(value);
            }
        }

        else if(name == "FAILURE") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            if(!map.check_event(value)) {
                std::cerr << "Event " << value << " has not been parsed before!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                failure = map.get_event(value);
            }
        }

        else if(name == "DIRECTION") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            direction = str_to_direction(value);
            if(direction == Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == "CONTINUOUS") {
            eResult = source->QueryBoolAttribute("value", &continuous);
            if (eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "TOLERANCE") {
            eResult = source->QueryIntAttribute("value", &tolerance);
            if (eResult != XML_SUCCESS) return eResult;
        }

        else {
            std::cerr << "Unknown event property \""<< p_name << "\" specified\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        source = source->NextSiblingElement("property");
    }
    if(event_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(success == nullptr && failure == nullptr) {
        std::cerr << "Trigger event has no single event to trigger!\n";
        return XML_ERROR_PARSING;
    }

    ActorEvent* event = create(success, failure, direction, continuous, tolerance);
    event->set_priority(prio);
    event->set_signal(sig);
    event->set_name(event_name);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}

/**
 * @brief When copied, also copy contained events
 */
ActorEvent* AteOnGround::copy() const{
    ActorEvent* success = nullptr;
    ActorEvent* failure = nullptr;
    if(m_success != nullptr) success = m_success->copy();
    if(m_failure != nullptr) failure = m_failure->copy();
    return create(success, failure, m_direction, m_continuous, m_tolerance);
}

/**
 * @brief Set cause to every contained event
 */
void AteOnGround::set_cause(Cause x) {
    ActorEvent::set_cause(x);
    if(m_success != nullptr) m_success->set_cause(x);
    if(m_failure != nullptr) m_failure->set_cause(x);
}

/**
 * @brief Set cause to every contained event
 */
void AteOnGround::set_key(SDL_Keysym x) {
    ActorEvent::set_key(x);
    if(m_success != nullptr) m_success->set_key(x);
    if(m_failure != nullptr) m_failure->set_key(x);
}
