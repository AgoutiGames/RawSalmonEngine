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
#include "event/ae_move.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "actor/data_block.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AeMove::m_alias = "AeMove";

Event<Actor>* AeMove::create(tinyxml2::XMLElement* source, MapData& base_map) const {
    Event<Actor>* event = new AeMove();
    tinyxml2::XMLError result = event->init(source, base_map);
    if(result != tinyxml2::XMLError::XML_SUCCESS) {return nullptr;}
    return event;
}

/**
 * @brief Increase speed of an actor
 * @param actor The actor which moves
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeMove::process(Actor& actor) {

    constexpr float FPS = 60;

    DataBlock& values = actor.get_data();

    bool collided = false;
    if(m_x_factor != 0.0f) {
        float temp = values.get_val_float(m_x_speed_name);
        if(temp != 0.0f) {
            if(!actor.move((temp * m_x_factor) / FPS, 0.0f, true)) {
                values.set_val(m_x_speed_name, 0.0f);
                collided = true;
            }
        }
    }

    if(m_y_factor != 0.0f) {
        float temp = values.get_val_float(m_y_speed_name);
        if(temp != 0.0f) {
            if(!actor.move(0.0f, (temp * m_y_factor) / FPS, true)) {
                values.set_val(m_y_speed_name, 0.0f);
                collided = true;
            }
        }
    }

    if(collided) {return EventSignal::abort;}
    else {return EventSignal::end;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMove::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;
    (void)base_map; // Mute unused var warning for seldomly used param MapData

    XMLError eResult;

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

        else if(name == "XFACTOR") {
            eResult = source->QueryFloatAttribute("value", &m_x_factor);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "XSPEED_NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            m_x_speed_name = std::string(p_value);
        }

        else if(name == "YFACTOR") {
            eResult = source->QueryFloatAttribute("value", &m_y_factor);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "YSPEED_NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            m_y_speed_name = std::string(p_value);
        }

        else {
            std::cerr << "Unknown event property \""<< p_name << "\" specified\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        source = source->NextSiblingElement("property");
    }
    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
