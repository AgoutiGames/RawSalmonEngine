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
#include "event/actor_event.hpp"

#include <iostream>
#include <string>

#include "event/event_container.hpp"
#include "event/ae_move_direction.hpp"
#include "event/ae_move_sustained.hpp"

std::map<std::string, ActorEvent*> ActorEvent::m_event_dict;

/// Fetch all available actor events and delete all currently stored actor event instances
void ActorEvent::initialize_all() {
    m_event_dict.clear();

    register_class<AeMoveDirection>();
    register_class<AeMoveSustained>();
}

/**
 * @brief Determine event type and parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError ActorEvent::parse_multi(tinyxml2::XMLElement* source, std::pair<std::string, ActorEvent*>& entry) {
    using namespace tinyxml2;
    XMLError eResult;

    const char* p_type;
    p_type = source->Attribute("type");

    std::string event_type;
    if(p_type != nullptr) {event_type = p_type;}
    else {
        return XML_NO_ATTRIBUTE;
    }

    if(m_event_dict.find(event_type) == m_event_dict.end()) {
        std::cerr << "Error: Cant parse event of unknown type " << event_type << "\n Add this event type to the ActorEvent initialize_all method!\n";
        return XML_WRONG_ATTRIBUTE_TYPE;
    }

    // Parse until first property element
    XMLElement* p_property = nullptr;
    XMLElement* p_properties = source->FirstChildElement("properties");
    if(p_properties != nullptr) {
        p_property = p_properties->FirstChildElement("property");
        if(p_property == nullptr) {
            std::cerr << "Error: Missing first property in event with id: " << source->Attribute("id") << "\n";
            return XML_ERROR_PARSING_ELEMENT;
        }
    }
    else {
        std::cerr << "Error: Missing properties in event with id: " << source->Attribute("id") << "\n";
        return XML_ERROR_PARSING_ELEMENT;
    }

    eResult = m_event_dict[event_type]->parse(p_property, entry);
    if(eResult != XML_SUCCESS) {std::cerr << "Failed at parsing event with tile id: " << source->Attribute("id") << "\n";}
    return eResult;
}

ActorEvent::~ActorEvent() {}
