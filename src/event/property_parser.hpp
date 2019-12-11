/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#ifndef PROPERTY_PARSER_HPP_INCLUDED
#define PROPERTY_PARSER_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>

#include "event/property_listener.hpp"
#include "util/game_types.hpp"
#include "util/logger.hpp"

/**
 * @brief A helper class which parses event properties and passes them to previously added references
 *
 * If the name of a value starts with an asteriks (*) then the this indicates that the name following
 * after is the name of a variable living inside a data block to be retrieved when running the event
 * the first time. The retrieval is done by the PropertyListener to which we register the variable name
 * and the event supplied references in case of an asteriks.
 */
 template<class EventType>
class PropertyParser{
    public:
        PropertyParser(PropertyListener<EventType>& property_listener, EventType& event);

        void add(bool EventType::* value, std::string name) {m_bool_e[name] = value;}
        void add(int EventType::* value, std::string name) {m_int_e[name] = value;}
        void add(float EventType::* value, std::string name) {m_float_e[name] = value;}
        void add(std::string EventType::* value, std::string name) {m_string_e[name] = value;}
        void add(PropertyType EventType::* type, bool EventType::* b, int EventType::* i, float EventType::* f, std::string EventType::* s, std::string name) {
            m_multi_e[name] = std::make_tuple(type, b, i, f, s);
        }

        void add(bool& value, std::string name) {m_bool[name] = &value;}
        void add(int& value, std::string name) {m_int[name] = &value;}
        void add(float& value, std::string name) {m_float[name] = &value;}
        void add(std::string& value, std::string name) {m_string[name] = &value;}
        void add(PropertyType& type, bool& b, int& i, float& f, std::string& s, std::string name) {
            m_multi[name] = std::make_tuple(&type, &b, &i, &f, &s);
        }

        void add(Priority& value, std::string name) {m_priority[name] = &value;}
        void add(EventSignal& value, std::string name) {m_signal[name] = &value;}
        void add(salmon::Direction& value, std::string name) {m_direction[name] = &value;}

        void add(std::vector<std::string>& value) {m_iterate = &value;}

        tinyxml2::XMLError parse(tinyxml2::XMLElement* source);
        tinyxml2::XMLError parse_ignore_unknown(tinyxml2::XMLElement* source);

    private:
        static constexpr int type_count = 14;
        PropertyListener<EventType>* m_property_listener;
        EventType* m_event;

        std::map<std::string, bool EventType::*> m_bool_e;
        tinyxml2::XMLError parse_bool_e(tinyxml2::XMLElement* source);

        std::map<std::string, int EventType::*> m_int_e;
        tinyxml2::XMLError parse_int_e(tinyxml2::XMLElement* source);

        std::map<std::string, float EventType::*> m_float_e;
        tinyxml2::XMLError parse_float_e(tinyxml2::XMLElement* source);

        std::map<std::string, std::string EventType::*> m_string_e;
        tinyxml2::XMLError parse_string_e(tinyxml2::XMLElement* source);

        std::map<std::string, std::tuple<PropertyType EventType::*,
                                         bool EventType::*,
                                         int EventType::*,
                                         float EventType::*,
                                         std::string EventType::*>> m_multi_e;

        tinyxml2::XMLError parse_multi_e(tinyxml2::XMLElement* source);


        std::map<std::string, bool*> m_bool;
        tinyxml2::XMLError parse_bool(tinyxml2::XMLElement* source);

        std::map<std::string, int*> m_int;
        tinyxml2::XMLError parse_int(tinyxml2::XMLElement* source);

        std::map<std::string, float*> m_float;
        tinyxml2::XMLError parse_float(tinyxml2::XMLElement* source);

        std::map<std::string, std::string*> m_string;
        tinyxml2::XMLError parse_string(tinyxml2::XMLElement* source);

        std::map<std::string, std::tuple<PropertyType*,bool*,int*,float*,std::string*>> m_multi;
        tinyxml2::XMLError parse_multi(tinyxml2::XMLElement* source);


        std::map<std::string, Priority*> m_priority;
        tinyxml2::XMLError parse_priority(tinyxml2::XMLElement* source);

        std::map<std::string, EventSignal*> m_signal;
        tinyxml2::XMLError parse_signal(tinyxml2::XMLElement* source);

        std::map<std::string, salmon::Direction*> m_direction;
        tinyxml2::XMLError parse_direction(tinyxml2::XMLElement* source);


        std::vector<std::string>* m_iterate = nullptr;
        tinyxml2::XMLError parse_iteration(tinyxml2::XMLElement* source);


        typedef tinyxml2::XMLError(PropertyParser::* ParsePointer)(tinyxml2::XMLElement* source);
        ParsePointer parsers[type_count];

};

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_bool_e(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_bool_e.find(name) == m_bool_e.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    std::string value(source->Attribute("value"));
    if(value.front() == '*') {
        // Remove star variable identifier
        value.erase(value.begin());
        m_property_listener->register_variable(value, m_bool_e.at(name));
        return XML_SUCCESS;
    }

    return source->QueryBoolAttribute("value", &(m_event->*m_bool_e.at(name)));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_int_e(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_int_e.find(name) == m_int_e.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    std::string value(source->Attribute("value"));
    if(value.front() == '*') {
        // Remove star variable identifier
        value.erase(value.begin());
        m_property_listener->register_variable(value, m_int_e.at(name));
        return XML_SUCCESS;
    }

    return source->QueryIntAttribute("value", &(m_event->*m_int_e.at(name)));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_float_e(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_float_e.find(name) == m_float_e.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    std::string value(source->Attribute("value"));
    if(value.front() == '*') {
        // Remove star variable identifier
        value.erase(value.begin());
        m_property_listener->register_variable(value, m_float_e.at(name));
        return XML_SUCCESS;
    }

    return source->QueryFloatAttribute("value", &(m_event->*m_float_e.at(name)));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_string_e(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_string_e.find(name) == m_string_e.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    std::string value(source->Attribute("value"));
    if(value.front() == '*') {
        // Remove star variable identifier
        value.erase(value.begin());
        m_property_listener->register_variable(value, m_string_e.at(name));
        return XML_SUCCESS;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_event->*m_string_e.at(name) = std::string(p_value);
    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_multi_e(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_multi_e.find(name) == m_multi_e.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    std::string value(source->Attribute("value"));
    if(value.front() == '*') {
        // Remove star variable identifier
        value.erase(value.begin());
        m_property_listener->register_variable(value, m_multi_e.at(name));
        return XML_SUCCESS;
    }

    auto& property = m_multi_e.at(name);
    std::string type = "";
    const char* p_type = source->Attribute("type");
    if(p_type != nullptr) {type = p_type;}

    if(type == "bool") {
        m_event->*std::get<0>(property) = PropertyType::Boolean;
        return source->QueryBoolAttribute("value", &(m_event->*std::get<1>(property)));
    }
    else if(type == "int") {
        m_event->*std::get<0>(property) = PropertyType::Integer;
        return source->QueryIntAttribute("value", &(m_event->*std::get<2>(property)));
    }
    else if(type == "float") {
        m_event->*std::get<0>(property) = PropertyType::Float;
        return source->QueryFloatAttribute("value", &(m_event->*std::get<3>(property)));
    }
    else if(type == "" || type == "file") {
        m_event->*std::get<0>(property) = PropertyType::String;
        const char* p_value = source->Attribute("value");
        if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_event->*std::get<4>(property) = std::string(p_value);
        return XML_SUCCESS;
    }
    else {
        Logger(Logger::error) << "Unknown type " << type << " specified! This shouldn't happen at all! Tiled must have messed up";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_bool(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_bool.find(name) == m_bool.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }
    return source->QueryBoolAttribute("value", m_bool.at(name));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_int(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_int.find(name) == m_int.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }
    return source->QueryIntAttribute("value", m_int.at(name));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_float(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_float.find(name) == m_float.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }
    return source->QueryFloatAttribute("value", m_float.at(name));
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_string(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_string.find(name) == m_string.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    *m_string.at(name) = std::string(p_value);
    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_multi(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_multi.find(name) == m_multi.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    auto& property = m_multi.at(name);
    std::string type(source->Attribute("type"));
    if(type == "bool") {
        *std::get<0>(property) = PropertyType::Boolean;
        return source->QueryBoolAttribute("value", std::get<1>(property));
    }
    else if(type == "int") {
        *std::get<0>(property) = PropertyType::Integer;
        return source->QueryIntAttribute("value", std::get<2>(property));
    }
    else if(type == "float") {
        *std::get<0>(property) = PropertyType::Float;
        return source->QueryFloatAttribute("value", std::get<3>(property));
    }
    else if(type == "" || type == "file") {
        *std::get<0>(property) = PropertyType::String;
        const char* p_value = source->Attribute("value");
        if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        *std::get<4>(property) = std::string(p_value);
        return XML_SUCCESS;
    }
    else {
        Logger(Logger::error) << "Unknown type " << type << " specified! This shouldn't happen at all! Tiled must have messed up";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_priority(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_priority.find(name) == m_priority.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    std::string value(p_value);
    Priority prio = str_to_priority(value);
    if(prio == Priority::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
    *m_priority.at(name) = prio;
    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_signal(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_signal.find(name) == m_signal.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    std::string value(p_value);
    EventSignal sig = str_to_event_signal(value);
    if(sig == EventSignal::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
    *m_signal.at(name) = sig;
    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_direction(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_direction.find(name) == m_direction.end()) {
        return XML_ERROR_MISMATCHED_ELEMENT;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    std::string value(p_value);
    salmon::Direction dir = str_to_direction(value);
    if(dir == salmon::Direction::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
    *m_direction.at(name) = dir;
    return XML_SUCCESS;
}

/**
 * @brief Check if attribute is known and parse by specified type
 * @param source The @c XMLElement which stores possibly matching attribute
 * @return @c XMLError which is XML_SUCESS if attribute is known and parsed properly
 *                              XML_ERROR_MISMATCHED_ELEMENT if attribute of this type isn't known
 *                              other XML_ERROR if something went wrong during parsing
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_iteration(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    if(m_iterate == nullptr) {return XML_ERROR_MISMATCHED_ELEMENT;}

    std::string name(source->Attribute("name"));
    if(name == std::to_string(m_iterate->size() + 1)) {
        const char* p_value = source->Attribute("value");
        if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_iterate->push_back(p_value);
        return XML_SUCCESS;
    }
    else {return XML_ERROR_MISMATCHED_ELEMENT;}
}

/// The constructor makes iteration through all parsing functions possible by adding them to a member function array
template<class EventType>
PropertyParser<EventType>::PropertyParser(PropertyListener<EventType>& property_listener, EventType& event) : m_property_listener{&property_listener}, m_event{&event} {
    parsers[0] = &PropertyParser::parse_bool;
    parsers[1] = &PropertyParser::parse_int;
    parsers[2] = &PropertyParser::parse_float;
    parsers[3] = &PropertyParser::parse_string;

    parsers[4] = &PropertyParser::parse_multi;

    parsers[5] = &PropertyParser::parse_int_e;
    parsers[6] = &PropertyParser::parse_float_e;
    parsers[7] = &PropertyParser::parse_string_e;
    parsers[8] = &PropertyParser::parse_bool_e;

    parsers[9] = &PropertyParser::parse_multi_e;

    parsers[10] = &PropertyParser::parse_priority;
    parsers[11] = &PropertyParser::parse_signal;
    parsers[12] = &PropertyParser::parse_direction;

    parsers[13] = &PropertyParser::parse_iteration;
    /// ! Don't forget adding new parsing member function here!
}

/**
 * @brief Iterate through all properties and parse them. If a property has an unknown name, return an error.
 * @param source The @c XMLElement which stores all attributes
 * @return @c XMLError which indicates sucess or failure
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    for(int counter = 0; source != nullptr; source = source->NextSiblingElement("property")) {
        bool no_match = true;

        // std::cerr << "Now Parsing " << source->Attribute("name") << "\n";
        if(source->Attribute("name") == nullptr) {
            Logger(Logger::error) << "Name of event property number: " << counter << " yields null!";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        for(ParsePointer p : parsers) {
            XMLError result = (this->*p)(source);
            if(result == XML_SUCCESS) {
                no_match = false;
                break;
            }
            else if(result == XML_ERROR_MISMATCHED_ELEMENT) {
                continue;
            }
            else {
                Logger(Logger::error) << "Failed parsing event property \"" << source->Attribute("name") << "\" number: " << counter;
                return result;
            }
        }

        if(no_match) {
            Logger(Logger::error) << "Unknown event property \"" << source->Attribute("name") << "\" number: " << counter;
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Iterate through all properties and parse them. If a property has an unknown name, ignore it.
          Only return error when no single event got parsed.
 * @param source The @c XMLElement which stores all attributes
 * @return @c XMLError which indicates sucess or failure
 */
 template<class EventType>
tinyxml2::XMLError PropertyParser<EventType>::parse_ignore_unknown(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    bool no_match = true;
    for(int counter = 0; source != nullptr; source = source->NextSiblingElement("property")) {

        if(source->Attribute("name") == nullptr) {
            Logger(Logger::error) << "Name of event property \"" << source->Attribute("name") << "\" number: " << counter << " yields null!";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        for(ParsePointer p : parsers) {
            XMLError result = (this->*p)(source);
            if(result == XML_SUCCESS) {
                no_match = false;
                break;
            }
            else if(result == XML_ERROR_MISMATCHED_ELEMENT) {
                continue;
            }
            else {
                Logger(Logger::error) << "Failed parsing event property \"" << source->Attribute("name") << "\" number: " << counter;
                return result;
            }
        }
    }
    if(no_match) {
        // std::cerr << "No single event got parsed!\n";
        return XML_NO_ATTRIBUTE;
    }
    return XML_SUCCESS;
}

#endif // PROPERTY_PARSER_HPP_INCLUDED
