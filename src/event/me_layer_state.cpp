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
#include "event/me_layer_state.hpp"

#include <string>
#include <map>
#include <iostream>

#include "map/mapdata.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "map/layer.hpp"
#include "map/object_layer.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string MeLayerState::m_alias = "MeLayerState";

const bool MeLayerState::good = Event<MapData>::register_class<MeLayerState>();

/**
 * @brief Do ...
 * @param MapData The MapData which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal MeLayerState::process(MapData& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    if(m_hide + m_unhide + m_suspend + m_unsuspend == 0) {
        std::cerr << "MeLayerState: " << m_name << " has no specified state change!\n";
        return EventSignal::abort;
    }
    if(m_hide && m_unhide) {
        std::cerr << "MeLayerState: " << m_name << " can't hide and unhide at the same time!\n";
        return EventSignal::abort;
    }
    if(m_suspend && m_unsuspend) {
        std::cerr << "MeLayerState: " << m_name << " can't suspend and unsuspend at the same time!\n";
        return EventSignal::abort;
    }
    Layer* layer = scope.get_layer_collection().get_layer(m_layer_name);
    if(layer == nullptr) {
        std::cerr << "MeLayerState: " << m_name << " cant find layer of name: " << m_layer_name << "\n";
        return EventSignal::abort;
    }
    if(m_hide) {layer->hide();}
    else if(m_unhide) {layer->unhide();}

    if(m_suspend || m_unsuspend) {
        if(layer->get_type() != Layer::object) {
            std::cerr << "MeLayerState: " << m_name << " can't un/suspend layer which isn't an object layer\n";
            return EventSignal::abort;
        }
        ObjectLayer* obl = static_cast<ObjectLayer*>(layer);

        if(m_suspend) {obl->suspend();}
        else if(m_unsuspend) {obl->unsuspend();}
    }
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch MapDatas or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError MeLayerState::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<MeLayerState> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    parser.add(&MeLayerState::m_layer_name, "LAYER_NAME");
    parser.add(&MeLayerState::m_hide, "HIDE");
    parser.add(&MeLayerState::m_unhide, "UNHIDE");
    parser.add(&MeLayerState::m_suspend, "SUSPEND");
    parser.add(&MeLayerState::m_unsuspend, "UNSUSPEND");

    XMLError eResult = parser.parse(source);

    if(m_name == "") {
        std::cerr << "Missing name property!\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing event: \"" << m_name << "\"\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
