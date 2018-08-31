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
#include "event/ae_accelerate.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "actor/data_block.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AeAccelerate::m_alias = "AeAccelerate";

AeAccelerate::AeAccelerate(float x_max_speed, float y_max_speed, float x_factor, float y_factor,std::string x_name, std::string y_name) :
EventContainer(),
m_x_max_speed{x_max_speed},
m_y_max_speed{y_max_speed},
m_x_factor{x_factor},
m_y_factor{y_factor},
m_x_speed_name{x_name},
m_y_speed_name{y_name}
{

}

/**
 * @brief Increase speed of an actor
 * @param actor The actor which moves
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeAccelerate::process(Actor& actor) {
    bool hit_limit = false;
    DataBlock& values = actor.get_data();
    if(m_x_factor != 0.0f) {

        float temp = values.get_val_float(m_x_speed_name);

        if(m_x_max_speed < 0.0f) {
            if(temp + m_x_factor < m_x_max_speed) {
                values.set_val(m_x_speed_name, m_x_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_x_speed_name, temp + m_x_factor);
            }
        }

        else {
            if(temp + m_x_factor > m_x_max_speed) {
                values.set_val(m_x_speed_name, m_x_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_x_speed_name, temp + m_x_factor);
            }
        }
    }

    if(m_y_factor != 0.0f) {
        float temp = values.get_val_float(m_y_speed_name);

        if(m_y_max_speed < 0.0f) {
            if(temp + m_y_factor < m_y_max_speed) {
                values.set_val(m_y_speed_name, m_y_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_y_speed_name, temp + m_y_factor);
            }
        }

        else {
            if(temp + m_y_factor > m_y_max_speed) {
                values.set_val(m_y_speed_name, m_y_max_speed);
                hit_limit = true;
            }
            else {
                values.set_val(m_y_speed_name, temp + m_y_factor);
            }
        }
    }
    if(hit_limit) {return EventSignal::abort;}
    else{return EventSignal::end;}
}

/// Create event and return pointer to it
AeAccelerate* AeAccelerate::create(float x_max_speed, float y_max_speed, float x_factor, float y_factor, std::string x_name, std::string y_name) {
    AeAccelerate temp(x_max_speed, y_max_speed, x_factor, y_factor, x_name, y_name);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeAccelerate::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    (void)map; // Mute unused var warning for seldomly used param MapData

    XMLError eResult;

    // Additional members
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

    float x_max_speed = 500.0f;
    float y_max_speed = 500.0f;
    float x_factor = 40.0f;
    float y_factor = 40.0f;
    std::string x_speed_name = "__XSPEED";
    std::string y_speed_name = "__YSPEED";

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

        else if(name == "XMAX_SPEED") {
            eResult = source->QueryFloatAttribute("value", &x_max_speed);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "YMAX_SPEED") {
            eResult = source->QueryFloatAttribute("value", &y_max_speed);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "XFACTOR") {
            eResult = source->QueryFloatAttribute("value", &x_factor);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "YFACTOR") {
            eResult = source->QueryFloatAttribute("value", &y_factor);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "XSPEED_NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            x_speed_name = std::string(p_value);
        }

        else if(name == "YSPEED_NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            y_speed_name = std::string(p_value);
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
    ActorEvent* event = create(x_max_speed,y_max_speed,x_factor,y_factor,x_speed_name,y_speed_name);
    event->set_priority(prio);
    event->set_signal(sig);
    event->set_name(event_name);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
