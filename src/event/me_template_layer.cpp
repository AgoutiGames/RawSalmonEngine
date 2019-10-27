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
#include "event/me_template_layer.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "actor/primitive.hpp"
#include "map/mapdata.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "map/object_layer.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string MeTemplateLayer::m_alias = "MeTemplateLayer";

const bool MeTemplateLayer::good = Event<MapData>::register_class<MeTemplateLayer>();

/**
 * @brief Do ...
 * @param MapData The MapData which should ...
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal MeTemplateLayer::process(MapData& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    if((m_purge && m_inject) || (!m_purge && !m_inject)) {
        Logger(Logger::error) << this << "can only either purge or inject!";
        return EventSignal::abort;
    }

    std::vector<ObjectLayer*> layers = scope.get_layer_collection().get_object_layers();
    ObjectLayer* dest_layer = nullptr;
    ObjectLayer* src_layer = nullptr;

    for(ObjectLayer* l : layers) {
        if(l->get_name() == m_destination) {
            dest_layer = l;
        }
        else if(l->get_name() == m_source) {
            src_layer = l;
        }
    }
    if(dest_layer == nullptr) {
        Logger(Logger::error) << this << "The name " << m_destination << " is no valid object layer";
        return EventSignal::abort;
    }
    else if(src_layer == nullptr) {
        Logger(Logger::error) << info() << " the name " << m_source << " is no valid object layer";
        return EventSignal::abort;
    }
    if(m_inject) {
        for(Actor* a : src_layer->get_actors()) {
            dest_layer->add_actor(*a);
        }
        for(Primitive* p : src_layer->get_primitives()) {
            dest_layer->add_primitive(p->clone());
        }
        return EventSignal::end;
    }
    else {
        bool success = true;
        for(Actor* a : src_layer->get_actors()) {
            if(!dest_layer->erase_actor(a->get_name())) {success = false;}
        }
        for(Primitive* p : src_layer->get_primitives()) {
            if(!dest_layer->erase_primitive(p->get_name())) {success = false;}
        }
        if(success) {return EventSignal::end;}
        else {return EventSignal::abort;}
    }
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch MapDatas or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError MeTemplateLayer::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<MeTemplateLayer> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");

    // Add additional members here
    parser.add(&MeTemplateLayer::m_inject, "INJECT");
    parser.add(&MeTemplateLayer::m_purge, "PURGE");
    parser.add(&MeTemplateLayer::m_source, "SOURCE");
    parser.add(&MeTemplateLayer::m_destination, "DESTINATION");

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
