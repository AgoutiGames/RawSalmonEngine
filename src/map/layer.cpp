/*
 * Copyright 2017-2020 Agouti Games Team (see the AUTHORS file)
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
#include <tinyxml2.h>

#include "map/layer_collection.hpp"
#include "map/map_layer.hpp"
#include "map/image_layer.hpp"
#include "map/object_layer.hpp"
#include "map/mapdata.hpp"
#include "util/logger.hpp"
#include "util/parse.hpp"

namespace salmon { namespace internal {

Layer::Layer(std::string name, LayerCollection* layer_collection) :
    m_layer_collection{layer_collection},
    m_name{name},
    m_transform{0,0,static_cast<float>(layer_collection->get_base_map().get_dimensions().w),
                    static_cast<float>(layer_collection->get_base_map().get_dimensions().h),0,0} {

}

/**
 * @brief Differentiates possible layers by name and calls proper parsing function
 * @param source The @c XMLElement which stores the layer information
 * @param layer_collection The @c LayerCollection to which the parsed layer belongs
 * @param eResult The @c XMLError which indicates sucess or failure of parsing
 * @return @c Layer The pointer to the parsed layer
 */
Layer* Layer::parse(tinyxml2::XMLElement* source, LayerCollection* layer_collection, tinyxml2::XMLError& eResult) {

    using namespace tinyxml2;

    // Parse layer name
    const char* p_name = source->Attribute("name");
    if (p_name == nullptr) {
        eResult = XML_ERROR_PARSING_ATTRIBUTE;
        return nullptr;
    }
    std::string name = std::string(p_name);

    // Don't check for errors because missing attribute means visible=1 / visible=true
    bool visible = true;
    source->QueryBoolAttribute("visible", &visible);

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
        Logger(Logger::error) << "Unknown layer type: " << source->Name() << " !";
        eResult = XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed loading Layer: " << name << " !";
        return nullptr;
    }

    if(!visible) {layer->hide();}

    return layer;

}

}} // namespace salmon::internal
