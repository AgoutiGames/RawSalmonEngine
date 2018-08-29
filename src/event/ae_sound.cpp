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
#include "event/ae_sound.hpp" //< Change this!

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "audio/sound_effect.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "map/mapdata.hpp"
#include "util/game_types.hpp"

std::string AeSound::m_alias = "AeSound";

AeSound::AeSound(SoundEffect sound) :
EventContainer(),
m_sound{sound}
{

}

/**
 * @brief Play a sound
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSound::process(Actor& actor) {
    (void)actor; // Mute unused var warning :
    m_sound.play();
    return EventSignal::end;
}

/// Create event and return pointer to it
AeSound* AeSound::create(SoundEffect sound) {
    AeSound temp(sound);
    return duplicate(temp);
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param entry Returns parsed event associated with its name
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSound::parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const{
    using namespace tinyxml2;
    (void)map; // Mute unused var warning for seldomly used param MapData
    //XMLError eResult;

    // Additional members
    std::string event_name("");
    Priority prio = Priority::medium;
    EventSignal sig = EventSignal::next;
    std::string path("");

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

        else if(name == "PATH") {
            p_value = source->Attribute("value");
            if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            path = std::string(p_value);
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
    else if(path == "") {
        std::cerr << "Missing path property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }
    SoundEffect temp(map.get_file_path() + "audio/" + path);
    if(!temp.good()) {
        std::cerr << "Audio file: " << map.get_file_path() + "audio/" + path << " not found!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    ActorEvent* event = create(temp);
    event->set_priority(prio);
    event->set_signal(sig);
    event->set_name(event_name);
    entry = std::make_pair(event_name, event);

    return XML_SUCCESS;
}
