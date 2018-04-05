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
#include "event/ae_fall.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

std::string AeFall::m_alias = "AeFall";

AeFall::AeFall(float acc, float max, unsigned death, Direction fall, AnimationType anim, Direction a_dir) :
EventContainer(),
m_acceleration{acc},
m_max_velocity{max},
m_death_height{death},
m_fall_dir{fall},
m_animation{anim},
m_anim_dir{a_dir}
{

}

/**
 * @brief Let the actor fall
 * @param actor The actor
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeFall::process(Actor& actor) {
    constexpr float FPS = 60;
    m_speed += m_acceleration / FPS;
    if(m_speed > m_max_velocity) {m_speed = m_max_velocity;}
    std::vector<float> way = dir_to_mov(m_fall_dir);
    float pos = actor.get_y();
    if(!actor.on_ground(Direction::down)) {
        actor.move(way[0] * m_speed / FPS, way[1] * m_speed / FPS, true);
        actor.block_event(m_alias);
        actor.animate(m_animation, m_anim_dir);
        std::cerr << "Fall: " << actor.get_y() - pos << " Actor pos x: "<< actor.get_x() << " y: " << actor.get_y() << "\n";
        m_height += abs(actor.get_y() - pos);
        return signal();
    }
    else {
        actor.unblock_event(m_alias);
        if(m_death_height != 0 && m_height >= m_death_height) {actor.respond(Response::on_death);}
        return EventSignal::end;
    }
}

/// Create event and return pointer to it
AeFall* AeFall::create(float acc, float max, unsigned death, Direction fall, AnimationType anim, Direction a_dir) {
    AeFall temp(acc, max, death, fall, anim, a_dir);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeFall::parse(tinyxml2::XMLElement* source, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    XMLError eResult;

    // Additional members
    std::string event_name("");

    float acceleration = 500.0f;
    float max_velocity = 1000.0f;
    unsigned death_height = 0;
    Direction fall_dir = Direction::down;
    AnimationType animation = AnimationType::fall;
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

        else if(name == "ACCELERATION") {
            eResult = source->QueryFloatAttribute("value", &acceleration);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "MAX_VELOCITY") {
            eResult = source->QueryFloatAttribute("value", &max_velocity);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "DEATH_HEIGHT") {
            eResult = source->QueryUnsignedAttribute("value", &death_height);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "FALL_DIRECTION") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string value(p_value);
            fall_dir = str_to_direction(value);
            if(fall_dir == Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
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
    ActorEvent* event = create(acceleration, max_velocity, death_height, fall_dir, animation, anim_dir);
    event->set_priority(prio);
    event->set_signal(sig);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
