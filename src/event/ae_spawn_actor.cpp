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
#include "event/ae_spawn_actor.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "map/layer.hpp"
#include "map/object_layer.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeSpawnActor::m_alias = "AeSpawnActor";

const bool AeSpawnActor::good = Event<Actor>::register_class<AeSpawnActor>();

/**
 * @brief Do ...
 * @param Actor The Actor which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSpawnActor::process(Actor& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);
    Layer* layer = scope.get_map().get_layer_collection().get_layer(m_layer_name);
    if(layer == nullptr) {
        Logger(Logger::error) << info() << " couldn't find layer with name " << m_layer_name;
        return EventSignal::abort;
    }
    if(layer->get_type() != Layer::object) {
        Logger(Logger::error) << info() << " layer " << m_layer_name << " is no valid object layer";
        return EventSignal::abort;
    }
    if(!scope.get_map().is_actor(m_actor_name)) {
        Logger(Logger::error) << info() << " actor " << m_actor_name << " doesn't exist";
        return EventSignal::abort;
    }
    Actor a = scope.get_map().get_actor(m_actor_name);
    a.set_name(m_spawn_name);
    a.move(scope.get_x() + m_x_pos, scope.get_y() + m_y_pos, true);
    ObjectLayer* obj_layer = static_cast<ObjectLayer*>(layer);
    obj_layer->add_actor(a);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSpawnActor::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeSpawnActor> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    parser.add(&AeSpawnActor::m_layer_name, "LAYER_NAME");
    parser.add(&AeSpawnActor::m_actor_name, "ACTOR_NAME");
    parser.add(&AeSpawnActor::m_x_pos, "X_POS");
    parser.add(&AeSpawnActor::m_y_pos, "Y_POS");
    parser.add(&AeSpawnActor::m_spawn_name, "SPAWN_NAME");

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
