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
#include "event/ae_move_static.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeMoveStatic::m_alias = "AeMoveStatic";

const bool AeMoveStatic::good = Event<Actor>::register_class<AeMoveStatic>();

/**
 * @brief Move the actor exactly to the specified word coords
 * @param Actor The Actor which should move statically
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 *
 * Values between 0 and 1 are interpreted as relative to the camera width and height
 * Values between -1 and 0 are relative but to the lower right corner of the camera
 * Other values are interpreted as pixel values
 * The origin of the actor is its upper left corner and for inverted relative valuse its lower right
 */
EventSignal AeMoveStatic::process(Actor& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    float x_movement, y_movement;
    Camera& cam = scope.get_map().get_camera();
    y_movement = scope.get_h();

    if(m_x_pos > 1.0f || m_x_pos < -1.0f) {
        x_movement = cam.x() + m_x_pos;
    }
    else if(m_x_pos > 0.0f) {
        x_movement = cam.x() + m_x_pos * cam.w();
    }
    else {
        x_movement = cam.x() + cam.w() + m_x_pos * cam.w();
        x_movement -= scope.get_w();
    }

    if(m_y_pos > 1.0f || m_y_pos < -1.0f) {
        y_movement = cam.y() + m_y_pos;
    }
    else if(m_y_pos > 0.0f) {
        y_movement = cam.y() + m_y_pos * cam.h();
    }
    else {
        y_movement = cam.y() + cam.h() + m_y_pos * cam.h();
        y_movement -= scope.get_h();
    }

    scope.move(x_movement, y_movement, true);

    return m_signal;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeMoveStatic::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeMoveStatic> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(m_signal, "SIGNAL");

    // Add additional members here
    parser.add(&AeMoveStatic::m_x_pos, "X_POS");
    parser.add(&AeMoveStatic::m_y_pos, "Y_POS");

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
