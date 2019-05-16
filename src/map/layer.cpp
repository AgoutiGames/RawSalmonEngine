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
#include "map/layer.hpp"

#include <list>
#include <string>
#include <sstream>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "map/map_layer.hpp"
#include "map/image_layer.hpp"
#include "map/object_layer.hpp"
#include "map/tile.hpp"
#include "util/base64.h"
#include "util/parse.hpp"
#include "util/tinyxml2.h"

Layer::Layer(std::string name, LayerCollection* layer_collection) {
    m_name = name;
    m_layer_collection = layer_collection;
}

Layer* Layer::parse(tinyxml2::XMLElement* source, LayerCollection* layer_collection, tinyxml2::XMLError& eResult) {

    using namespace tinyxml2;

    // Parse layer name
    const char* p_name = source->Attribute("name");
    if (p_name == nullptr) {
        eResult = XML_ERROR_PARSING_ATTRIBUTE;
        return nullptr;
    }
    std::string name = std::string(p_name);

    Layer* layer;

    // Parse map type "map"
    if(std::string("layer") == source->Name()) {
        layer = MapLayer::parse(source, name, layer_collection, eResult);
    }

    // Parse map type "object"
    else if(std::string("objectgroup") == source->Name()) {
        layer = ObjectLayer::parse(source, name, layer_collection, eResult);
    }

    // Parse map type "image"
    else if(std::string("imagelayer") == source->Name()) {
        layer = ImageLayer::parse(source, name, layer_collection, eResult);
    }

    // Return error for unknown layer types
    else {
        std::cerr << "Unknown layer type: " << source->Name() << " !\n";
        eResult = XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        std::cerr << "Error loading Layer: " << name << " !\n";
        return nullptr;
    }

    return layer;

}

