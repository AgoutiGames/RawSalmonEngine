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
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);


    if(!m_cause.mouse()) {
        Logger(Logger::error) << info() << " not triggered by mouse event!";
        return EventSignal::abort;
    }

    if(m_mouse_button_index > 5 || m_mouse_button_index < 1) {
        Logger(Logger::error) << info() << " triggers on invalid mouse button number: " << m_mouse_button_index;
        return EventSignal::abort;
    }
    MouseState mouse = scope.get_map().get_input_handler().get_mouse_state();

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

    // If one supplied button state matches the setting return end, else return abort

    if(m_pressed && button.pressed) {return EventSignal::end;}
    else if(m_released && button.released) {return EventSignal::end;}
    else if(m_down && button.down) {return EventSignal::end;}
    else {return EventSignal::abort;}

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

    std::string success_event, failure_event;

    // Add additional members here
    parser.add(&AteMouseButtons::m_mouse_button_index, "MOUSE_BUTTON_INDEX");
    parser.add(&AteMouseButtons::m_pressed, "PRESSED");
    parser.add(&AteMouseButtons::m_released, "RELEASED");
    parser.add(&AteMouseButtons::m_down, "DOWN");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
