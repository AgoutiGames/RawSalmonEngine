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
#ifndef EVENT_HPP_INCLUDED
#define EVENT_HPP_INCLUDED

#include <iostream>
#include <string>
#include <map>
#include <tinyxml2.h>

#include "actor/collision.hpp"
#include "util/game_types.hpp"
#include "util/logger.hpp"

class MapData;

/**
 * @brief The game events which carry additional information and operate on their Scope type
 */
template<class Scope>
class Event {
public:
    Event() = default;
    // Enable possible copy or move of derived
    Event(const Event& other) = default;
    Event& operator=(const Event& other) = default;
    Event(Event&& other) = default;
    Event& operator=(Event&& other) = default;

    // delete works on pointer to base
    virtual ~Event() = default;

    virtual tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) = 0;
    virtual EventSignal process(Scope& entity) = 0;
    virtual std::string get_type() const = 0;

    virtual Event<Scope>* clone() const = 0;

    std::string get_name() const {return m_name;}
    Priority get_priority() const {return m_priority;}
    EventSignal get_signal() const {return m_signal;}

    /// Useful function for printing event id
    std::string info() const {return get_type() + ": " + get_name();}

    virtual void set_collision(Collision x) {m_collision = x;}
    const Collision& get_collision() const {return m_collision;}

    static Event<Scope>* parse(tinyxml2::XMLElement* source, MapData& base_map);

protected:
    std::string m_name = "";
    Priority m_priority = Priority::medium;
    EventSignal m_signal = EventSignal::next;
    Collision m_collision;

    template <class T>
    static bool register_class();

private:

    static std::map<std::string, Event<Scope>*>& get_dict();

};

/// Return the dictionary which translates the event typ name to an empty instace of the event type
template<class Scope>
std::map<std::string, Event<Scope>*>& Event<Scope>::get_dict() {
    // Lazy initialization of event_dict
    // Can be called during dynamic initialization of static members
    // of other classes (events can finally register themselves before main!)
    static std::map<std::string, Event<Scope>*> event_dict;
    return event_dict;
}

/// Registers the type of event by its name
template <class Scope>
template <class T>
bool Event<Scope>::register_class() {
    Event<Scope>* event = new T();
    get_dict()[event->get_type()] = event;
    // std::cerr << "Just registered " << event->get_type() << "\n";
    return true;
}

/**
 * @brief Differentiates events by type, forwards to individual parsing functions and returns the ready event
 * @param source The @c XMLElement which contains the event information
 * @param base_map The map to which our event belongs
 * @return @c Event<Scope>* which owns the parsed event
 */
template<class Scope>
Event<Scope>* Event<Scope>::parse(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    const char* p_type;
    p_type = source->Attribute("type");

    std::string event_type;
    if(p_type != nullptr) {event_type = p_type;}
    else {
        return nullptr;
    }

    if(get_dict().find(event_type) == get_dict().end()) {
        Logger(Logger::error) << "Cant parse event of unknown type: " << event_type;
        return nullptr;
    }

    // Parse until first property element
    XMLElement* p_property = nullptr;
    XMLElement* p_properties = source->FirstChildElement("properties");
    if(p_properties != nullptr) {
        p_property = p_properties->FirstChildElement("property");
        if(p_property == nullptr) {
            Logger(Logger::error) << "Missing first property in event with id: " << source->Attribute("id");
            return nullptr;
        }
    }
    else {
        Logger(Logger::error) << "Error: Missing properties in event with id: " << source->Attribute("id");
        return nullptr;
    }

    Event<Scope>* parsed_event = get_dict()[event_type]->clone();
    tinyxml2::XMLError result = parsed_event->init(p_property, base_map);
    if(result != tinyxml2::XMLError::XML_SUCCESS) {
        Logger(Logger::error) << "Failed at parsing event of type:" << parsed_event->get_type() << " with tile id: " << source->Attribute("id");
        delete parsed_event;
        return nullptr;
    }
    return parsed_event;
}

#endif // EVENT_HPP_INCLUDED
