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
#include "event/ae_move_sustained.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

std::string AeMoveSustained::m_alias = "AeMoveSustained";

AeMoveSustained::AeMoveSustained(Direction dir) :
EventContainer(),
m_direction{dir}
{

}

/**
 * @brief Move the actor to the supplied direction until key is released
 * @param actor The actor which should move
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMoveSustained::process(Actor& actor) {
    // process stuff
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[get_key().scancode]) {
        std::vector<float> mov_factors = dir_to_mov(m_direction);
        if(actor.move(mov_factors[0], mov_factors[1])) {
            actor.animate(AnimationType::walk, m_direction);
        }
        else {
            actor.animate(AnimationType::idle, m_direction);
        }
        actor.block_event(m_alias);
        //actor.set_cooldown(m_alias, 0.5);
        return signal();
    }
    else {
        actor.unblock_event(m_alias);
        return EventSignal::end;
    }
}

/// Create event and return pointer to it
AeMoveSustained* AeMoveSustained::create(Direction dir) {
    AeMoveSustained temp(dir);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMoveSustained::parse(tinyxml2::XMLElement* source, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    //XMLError eResult;

    Direction dir = Direction::up;
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

    while(source != nullptr) {
        const char* p_name;
        const char* p_value;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        else if(name == "DIRECTION") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            dir = str_to_direction(value);
            if(dir == Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

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
    ActorEvent* event = create(dir);
    event->set_priority(prio);
    event->set_signal(sig);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
