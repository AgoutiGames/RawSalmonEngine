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
#include "event/ae_set_var.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AeSetVar::m_alias = "AeSetVar";

AeSetVar::AeSetVar(std::string name, bool data, bool global) :
EventContainer(),
m_name{name},
m_global{global}
{
    m_value.type = Value::Bool;
    m_value.b = data;
}

AeSetVar::AeSetVar(std::string name, int data, bool global, bool add, bool mult) :
EventContainer(),
m_name{name},
m_global{global},
m_add{add},
m_mult{mult}
{
    m_value.type = Value::Int;
    m_value.i = data;
}

AeSetVar::AeSetVar(std::string name, float data, bool global, bool add, bool mult) :
EventContainer(),
m_name{name},
m_global{global},
m_add{add},
m_mult{mult}
{
    m_value.type = Value::Float;
    m_value.f = data;
}

AeSetVar::AeSetVar(std::string name, std::string data, bool global, bool add) :
EventContainer(),
m_name{name},
m_global{global},
m_add{add}
{
    m_value.type = Value::String;
    m_value.s = data;
}

/// Create event and return pointer to it
AeSetVar* AeSetVar::create(std::string name, bool data, bool global) {
    AeSetVar temp(name, data, global);
    return duplicate(temp);
}

/// Create event and return pointer to it
AeSetVar* AeSetVar::create(std::string name, int data, bool global, bool add, bool mult) {
    AeSetVar temp(name, data, global, add, mult);
    return duplicate(temp);
}

/// Create event and return pointer to it
AeSetVar* AeSetVar::create(std::string name, float data, bool global, bool add, bool mult) {
    AeSetVar temp(name, data, global, add, mult);
    return duplicate(temp);
}

/// Create event and return pointer to it
AeSetVar* AeSetVar::create(std::string name, std::string data, bool global, bool add) {
    AeSetVar temp(name, data, global, add);
    return duplicate(temp);
}
/**
 * @brief Do ...
 * @param actor The actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSetVar::process(Actor& actor) {
    DataBlock& data = (m_global) ? actor.get_map().get_data() : actor.get_data();
    switch(m_value.type) {

        case Value::Bool : {
            data.set_val(m_name, m_value.b);
        }
        break;

        case Value::Int : {
            if(m_add) {
                int temp = data.get_val_int(m_name);
                data.set_val(m_name, temp + m_value.i);
            }
            else if(m_mult) {
                int temp = data.get_val_int(m_name);
                data.set_val(m_name, temp * m_value.i);
            }
            else {data.set_val(m_name, m_value.i);}
        }
        break;

        case Value::Float : {
            if(m_add) {
                float temp = data.get_val_float(m_name);
                data.set_val(m_name, temp + m_value.f);
            }
            else if(m_mult) {
                float temp = data.get_val_float(m_name);
                data.set_val(m_name, temp * m_value.f);
            }
            else {data.set_val(m_name, m_value.f);}
        }
        break;

        case Value::String : {
            if(m_add) {
                std::string temp = data.get_val_string(m_name);
                data.set_val(m_name, temp + m_value.s);
            }
            else {data.set_val(m_name, m_value.s);}
        }
        break;

        case Value::undefined : {
            std::cerr << "The actor " << actor.get_name() << " called AeSetVar: " << name() << " with an undefined value!\n";
            return EventSignal::abort;
        }
    }
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSetVar::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    (void)map; // Mute unused var warning for seldomly used param MapData
    XMLError eResult = XML_SUCCESS;

    // Additional members
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;

    std::string val_name("");
    Value::Type type = Value::Float;
    bool value_b;
    int value_i;
    float value_f;
    std::string value_s;
    bool local = true;
    bool global = false;
    bool add = false;
    bool mult = false;

    tinyxml2::XMLElement* temp = source;

    while(source != nullptr) {
        const char* p_name;
        const char* p_value;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse additional members

        else if(name == "TYPE") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            std::string str_type{p_value};
            if(str_type == "BOOL") {
                type = Value::Bool;
            }
            else if(str_type == "INT") {
                type = Value::Int;
            }
            else if(str_type == "FLOAT") {
                type = Value::Float;
            }
            else if(str_type == "STRING") {
                type = Value::String;
            }
            else {
                std::cerr << "Event holds unknown type " << str_type << "\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
        }
        source = source->NextSiblingElement("property");
    }
    if(type == Value::undefined) {
        std::cerr << "Event is missing TYPE field!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    source = temp;

    while(source != nullptr) {
        const char* p_name;
        const char* p_value;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse additional members
        else if(name == "TYPE") {}

        else if(name == "NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            event_name = std::string(p_value);
        }

        else if(name == "VAL_NAME") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            val_name = std::string(p_value);
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

        else if(name == "LOCAL") {
            eResult = source->QueryBoolAttribute("value", &local);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "GLOBAL") {
            eResult = source->QueryBoolAttribute("value", &global);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "+=") {
            eResult = source->QueryBoolAttribute("value", &add);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "*=") {
            eResult = source->QueryBoolAttribute("value", &mult);
            if(eResult != XML_SUCCESS) return eResult;
        }

        else if(name == "VALUE") {
            switch(type) {
            case(Value::Bool) : eResult = source->QueryBoolAttribute("value", &value_b); break;
            case(Value::Int) : eResult = source->QueryIntAttribute("value", &value_i); break;
            case(Value::Float) : eResult = source->QueryFloatAttribute("value", &value_f); break;
            case(Value::String) : {
                p_value = source->Attribute("value");
                if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                value_s = std::string(p_value);
            }break;
            case(Value::undefined) : {
                std::cerr << "This shouldn't ever happen!\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            }
            if(eResult != XML_SUCCESS) {
                std::cerr << "Problem encountered with parsing \"VALUE\" of the set_var event\n";
                return eResult;
            }
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

    if(val_name == "") {
        std::cerr << "Missing value name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if( (local && global) || (!local && !global) ) {
        std::cerr << "You must either set global or local var, neither both nor none!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    ActorEvent* event;

    if(type == Value::Bool) {
        if(add || mult) {
            std::cerr << "ae_set_var of type bool can't handle adding or multiplying instruction!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        event = create(val_name, value_b, global);
    }
    else if(type == Value::Int) {
        event = create(val_name, value_i, global, add, mult);
    }
    else if(type == Value::Float) {
        event = create(val_name, value_f, global, add, mult);
    }
    else if(type == Value::String) {
        if(mult) {
            std::cerr << "ae_set_var of type string can concatenate via += but NOT multiply strings!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        event = create(val_name, value_s, global, add);
    }
    else {std::cerr << "This shouldn't ever happen!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }


    event->set_priority(prio);
    event->set_signal(sig);
    event->set_name(event_name);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
