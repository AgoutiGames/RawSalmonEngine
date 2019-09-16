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
#include "event/ate_mouse_buttons.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AteMouseButtons::m_alias = "AteMouseButtons";

const bool AteMouseButtons::good = Event<Actor>::register_class<AteMouseButtons>();

/**
 * @brief Do ...
 * @param Actor The Actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AteMouseButtons::process(Actor& scope) {
    // Setup if event should trigger
    if(m_start) {
        // Syncs members with possibly linked DataBlock variables
        listen(m_property_listener, *this, scope);

        if(!m_cause.mouse()) {
            std::cerr << "Event: " << m_name << " of type AteMouse not triggered by mouse event!\n";
            return EventSignal::abort;
        }
        if(m_mouse_button_index > 5 || m_mouse_button_index < 1) {
            std::cerr << "Event: " << m_name << " triggers on invalid mouse button number: " << m_mouse_button_index << "\n";
            return EventSignal::abort;
        }
        MouseState mouse = m_cause.get_mouse();
        MouseButtonState button;
        switch(m_mouse_button_index) {
            case 1: {button = mouse.left; break;}
            case 2: {button = mouse.middle; break;}
            case 3: {button = mouse.right; break;}
            case 4: {button = mouse.extra1; break;}
            case 5: {button = mouse.extra2; break;}
            // This never happens
            default: {return EventSignal::abort;}
        }
        m_decision = false;
        if(m_pressed && button.pressed) {m_decision = true;}
        else if(m_released && button.released) {m_decision = true;}
        else if(m_down && button.down) {m_decision = true;}

        m_start = false;
    }

    EventSignal sig;

    if(m_decision) {
        if(!m_success) {return EventSignal::end;}
        sig = m_success->process(scope);
    }
    else {
        if(!m_failure) {return EventSignal::end;}
        sig = m_failure->process(scope);
    }

    if(sig == EventSignal::next) return get_signal();
    else {return sig;}
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AteMouseButtons::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AteMouseButtons> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    std::string success_event, failure_event;

    // Add additional members here
    parser.add(&AteMouseButtons::m_mouse_button_index, "MOUSE_BUTTON_INDEX");
    parser.add(&AteMouseButtons::m_pressed, "PRESSED");
    parser.add(&AteMouseButtons::m_released, "RELEASED");
    parser.add(&AteMouseButtons::m_down, "DOWN");
    parser.add(success_event, "SUCCESS");
    parser.add(failure_event, "FAILURE");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(success_event != "") {
        if(!base_map.check_event_convert_actor(success_event)) {
            std::cerr << "Event " << success_event << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_success = base_map.get_event_convert_actor(success_event);
        }
    }

    if(failure_event != "") {
        if(!base_map.check_event_convert_actor(failure_event)) {
            std::cerr << "Event " << failure_event << " has not been parsed before!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_failure = base_map.get_event_convert_actor(failure_event);
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Set cause to every contained event
 */
void AteMouseButtons::set_cause(Cause x) {
    Event<Actor>::set_cause(x);
    if(m_success) m_success->set_cause(x);
    if(m_failure) m_failure->set_cause(x);
}
