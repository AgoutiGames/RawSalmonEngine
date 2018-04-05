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
#include "event/ae_animate.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

std::string AeAnimate::m_alias = "AeAnimate";

AeAnimate::AeAnimate(AnimationType anim, Direction dir, unsigned cyc, unsigned a_f, unsigned g_f) :
EventContainer(),
m_animation{anim},
m_direction{dir},
m_cycles{cyc},
m_anim_frames{a_f},
m_game_frames{g_f}
{

}

/**
 * @brief Animate the actor
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 * @note Adds up cycles, anim frames and game frames
 */
EventSignal AeAnimate::process(Actor& actor) {
    AnimSignal sig = actor.animate_trigger(m_animation, m_direction);
    if(m_cycles > 0) {
        if(sig == AnimSignal::wrap) {
            m_cycles--;
        }
    }
    else if(m_anim_frames > 0) {
        if(sig != AnimSignal::none) {
            m_anim_frames --;
        }
    }
    else if(m_game_frames > 0) {
        m_game_frames--;
    }
    else {return EventSignal::end;}

    return signal();
}

/// Create event and return pointer to it
AeAnimate* AeAnimate::create(AnimationType anim, Direction dir, unsigned cyc, unsigned a_f, unsigned g_f) {
    AeAnimate temp(anim, dir, cyc, a_f, g_f);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeAnimate::parse(tinyxml2::XMLElement* source, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    XMLError eResult;

    // Additional members
    AnimationType anim = AnimationType::idle;
    Direction dir = Direction::current;
    unsigned cycles = 0;
    unsigned anim_frames = 0;
    unsigned game_frames = 0;
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

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

        else if(name == "ANIMATION_TYPE") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            anim = str_to_anim_type(value);
            if(anim == AnimationType::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
            if(anim == AnimationType::none) {
                std::cerr << "Cant count cycles or frames with Animation Type none!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
        }

        else if(name == "DIRECTION") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            dir = str_to_direction(value);
            if(dir == Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == "CYCLES") {
            eResult = source->QueryUnsignedAttribute("value", &cycles);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "ANIMATION_FRAMES") {
            eResult = source->QueryUnsignedAttribute("value", &anim_frames);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "GAME_FRAMES") {
            eResult = source->QueryUnsignedAttribute("value", &game_frames);
            if(eResult != XML_SUCCESS) return eResult;
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
    ActorEvent* event = create(anim, dir, cycles, anim_frames, game_frames);
    event->set_priority(prio);
    event->set_signal(sig);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
