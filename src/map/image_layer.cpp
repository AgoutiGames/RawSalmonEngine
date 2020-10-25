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
#include "map/image_layer.hpp"

#include <iostream>

#include "map/mapdata.hpp"
#include "util/parse.hpp"
#include "util/logger.hpp"
#include "core/gameinfo.hpp"

namespace salmon { namespace internal {

/// Factory function which retrieves a pointer owning the image layer
ImageLayer* ImageLayer::parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) {
    return new ImageLayer(source, name, layer_collection, eresult);
}

/// Constructor which only may be used internally
ImageLayer::ImageLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) : Layer(name, layer_collection) {
    eresult = init(source);
}

/**
 * @brief Initialize the layer by parsing info from source
 * @param source The @c XMLElement from which information is parsed
 * @return @c XMLError which indicates failure or sucess of parsing
 */
tinyxml2::XMLError ImageLayer::init(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    float offset_x, offset_y;
    // Parse image position
    eResult = source->QueryFloatAttribute("offsetx", &offset_x);
    if(eResult != XML_SUCCESS) {
        offset_x = 0;
    }
    eResult = source->QueryFloatAttribute("offsety", &offset_y);
    if(eResult != XML_SUCCESS) {
        offset_y = 0;
    }
    // Parse opacity (which works with image layer)
    float opacity;
    eResult = source->QueryFloatAttribute("opacity", &opacity);
    if(eResult == XML_SUCCESS) m_opacity = opacity;

    // Parse image file path
    XMLElement* p_image = source->FirstChildElement("image");
    if(p_image == nullptr) return XML_ERROR_PARSING_ELEMENT;
    const char* p_source = p_image->Attribute("source");
    if(p_source == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    MapData& base_map = m_layer_collection->get_base_map();
    m_img_src = base_map.get_file_path() + std::string(p_source);

    const char* p_color_key;
    p_color_key = p_image->Attribute("trans");
    // Parse the actual image data
    if (p_color_key == nullptr) {
        m_img = base_map.get_game().get_texture_cache().get(m_img_src);
    }
    else {
        std::string color_string = p_color_key;
        SDL_Color color_key = str_to_color(color_string);
        m_img = base_map.get_game().get_texture_cache().get(m_img_src,color_key);
    }

    if(!m_img.valid()) {
        Logger(Logger::error) << "Failed to load image layer: " << m_name << " image file: " << m_img_src;
        return XML_ERROR_PARSING;
    }
    m_img.setAlpha(static_cast<Uint8>(m_opacity * 255));

    m_transform = salmon::Transform(offset_x,offset_y, m_img.getWidth(),m_img.getHeight(),0,0);

    // Parse image properties (only blend mode right now)
    XMLElement* p_properties = source->FirstChildElement("properties");
    if(p_properties != nullptr) {
        XMLElement* p_property = p_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            if(name == "BLEND_MODE") {
                eResult = parse::blendmode(p_property, m_img);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Failed at parsing blend mode for layer: " << m_name;
                    return eResult;
                }
            }
            else{
                Logger(Logger::error) << "Unknown image layer property " << p_name << " occured";
                return XML_ERROR_PARSING;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Renders the image layer to screen according to camera
 * @param camera Our active camera
 * @return @c bool which indicates sucess
 */
bool ImageLayer::render(const Camera& camera) const {
    if(m_hidden || !camera.get_transform().to_rect().has_intersection(m_transform.to_rect())) {return true;}
    else {
        SDL_Rect clip{0,0,m_img.getWidth(),m_img.getHeight()};
        SDL_Rect dest{make_rect(m_transform.to_rect())};
        m_img.render_resize(&clip,&dest);
        return true;
    }
}

}} // namespace salmon::internal
