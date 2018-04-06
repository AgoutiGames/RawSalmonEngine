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
#include "event/ae_jump.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "event/ae_fall.hpp"
#include "util/game_types.hpp"

std::string AeJump::m_alias = "AeJump";

AeJump::AeJump(float dur, float j_h, bool slow_r, float slow_f, AnimationType anim, Direction anim_dir) :
EventContainer(),
m_duration{dur},
m_jump_height{j_h},
m_slow_on_release{slow_r},
m_slow_factor{slow_f},
m_animation{anim},
m_anim_dir{anim_dir}
{

}

/**
 * @brief Perform a jump
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeJump::process(Actor& actor) {
    if(!m_started) {
        actor.scrap_event(AeFall::get_type_static());
        actor.block_event(AeFall::get_type_static());
        m_started = true;
    }

    constexpr float FPS = 60;

    if(m_deceleration == 0) {
        float steps = FPS * m_duration;
        steps = m_jump_height / steps;
        m_speed = steps * 2;
        m_deceleration = m_speed / (FPS * m_duration);
    }

    if(!actor.move(0, -m_speed, true)) {
        actor.unblock_event(AeFall::get_type_static());
        return EventSignal::abort;
    }

    actor.animate(m_animation, m_anim_dir);

    if(m_slow_on_release) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if ( !(keys[get_key().scancode]) ) {
            m_deceleration = m_speed * m_slow_factor + m_deceleration;
            m_slow_on_release = false;
        }
    }

    m_speed -= m_deceleration;

    if(m_speed <= 0) {
        actor.unblock_event(AeFall::get_type_static());
        return EventSignal::end;
    }
    else {return signal();}
}

/// Create event and return pointer to it
AeJump* AeJump::create(float dur, float j_h, bool slow_r, float slow_f, AnimationType anim, Direction anim_dir) {
    AeJump temp(dur, j_h, slow_r, slow_f, anim, anim_dir);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeJump::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    (void)map; // Mute unused var warning for seldomly used param MapData
    XMLError eResult;

    // Additional members
    std::string event_name("");

    float duration = 0.5f;
    float jump_height = 400.0f;
    bool slow_on_release = true;
    float slow_factor = 0.5f;

    AnimationType animation = AnimationType::jump;
    Direction anim_dir = Direction::current;

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

        else if(name == "JUMP_DURATION") {
            eResult = source->QueryFloatAttribute("value", &duration);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "JUMP_HEIGHT") {
            eResult = source->QueryFloatAttribute("value", &jump_height);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "SLOW_ON_RELEASE") {
            eResult = source->QueryBoolAttribute("value", &slow_on_release);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "SLOW_FACTOR") {
            eResult = source->QueryFloatAttribute("value", &slow_factor);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "ANIMATION_TYPE") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            animation = str_to_anim_type(value);
            if(animation == AnimationType::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
        }

        else if(name == "ANIMATION_DIRECTION") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            anim_dir = str_to_direction(value);
            if(anim_dir == Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
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
    ActorEvent* event = create(duration,jump_height,slow_on_release,slow_factor,animation,anim_dir);
    event->set_priority(prio);
    event->set_signal(sig);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
