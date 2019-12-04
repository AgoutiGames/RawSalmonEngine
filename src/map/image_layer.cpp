/*
 * Copyright 2017-201 Agouti Games Team (see the AUTHORS file)
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

    // Parse image position
    eResult = source->QueryFloatAttribute("offsetx", &m_offset_x);
    if(eResult != XML_SUCCESS) {
        m_offset_x = 0;
    }
    eResult = source->QueryFloatAttribute("offsety", &m_offset_y);
    if(eResult != XML_SUCCESS) {
        m_offset_y = 0;
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

    // Parse the actual image data
    m_img = base_map.get_game().get_texture_cache().get(m_img_src);
    if(!m_img.valid()) {
        Logger(Logger::error) << "Failed to load image layer: " << m_name << " image file: " << m_img_src;
        return XML_ERROR_PARSING;
    }
    m_img.setAlpha(static_cast<Uint8>(m_opacity * 255));
    m_width = static_cast<unsigned>(m_img.getWidth());
    m_height = static_cast<unsigned>(m_img.getHeight());

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
            else if(name == "PARALLAX") {
                eResult = p_property->QueryBoolAttribute("value", &m_parallax);
                if(eResult != XML_SUCCESS) return eResult;
            }
            else if(name == "STATIC") {
                eResult = p_property->QueryBoolAttribute("value", &m_static);
                if(eResult != XML_SUCCESS) return eResult;
            }
            else if(name == "STRETCH") {
                eResult = p_property->QueryBoolAttribute("value", &m_stretch);
                if(eResult != XML_SUCCESS) return eResult;
            }
            else if(name == "KEEP_SIZE") {
                eResult = p_property->QueryBoolAttribute("value", &m_keep_size);
                if(eResult != XML_SUCCESS) return eResult;
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
    if(m_hidden) {return true;}
    if(m_parallax) {
        MapData& base_map = m_layer_collection->get_base_map();
        int x_range = base_map.get_w() - camera.w();
        int y_range = base_map.get_h() - camera.h();
        float x_trans_fact = static_cast<float>(camera.x()) / x_range;
        float y_trans_fact = static_cast<float>(camera.y()) / y_range;
        x_trans_fact = x_trans_fact * (m_width - camera.w());
        y_trans_fact = y_trans_fact * (m_height - camera.h());
        m_img.render(-x_trans_fact, -y_trans_fact);
    }
    else if(m_static) {
        if(m_stretch) {
            SDL_Rect full{0,0,m_img.getWidth(),m_img.getHeight()};
            GameInfo& game = m_layer_collection->get_base_map().get_game();
            SDL_Rect dest{0,0,
            static_cast<int>(game.get_game_x_resolution()),
            static_cast<int>(game.get_game_y_resolution())};
            m_img.render_resize(&full,&dest);
        }
        else {
            int x_pos;
            int y_pos;
            int width;
            int height;
            // Size relative to window, not resolution
            if(m_keep_size) {
                GameInfo& game = m_layer_collection->get_base_map().get_game();
                float x_ratio = static_cast<float>(game.get_game_x_resolution()) / game.get_window_x_resolution();
                float y_ratio = static_cast<float>(game.get_game_y_resolution()) / game.get_window_y_resolution();
                width = m_img.getWidth() * x_ratio;
                height = m_img.getHeight() * y_ratio;
            }
            else {
                width = m_img.getWidth();
                height = m_img.getHeight();
            }
            if(m_offset_x > 0.0f && m_offset_x < 1.0f) {
                GameInfo& game = m_layer_collection->get_base_map().get_game();
                x_pos = static_cast<int>(m_offset_x * game.get_game_x_resolution()) - width/2;
            }
            else {
                x_pos = static_cast<int>(m_offset_x);
            }
            if(m_offset_y > 0.0f && m_offset_y < 1.0f) {
                GameInfo& game = m_layer_collection->get_base_map().get_game();
                y_pos = static_cast<int>(m_offset_y * game.get_game_y_resolution()) - height/2;
            }
            else {
                y_pos = static_cast<int>(m_offset_y);
            }

            SDL_Rect full{0,0,m_img.getWidth(),m_img.getHeight()};
            SDL_Rect dest{x_pos,y_pos,width,height};
            m_img.render_resize(&full,&dest);
        }
    }
    else{
        int x = m_offset_x - camera.x();
        int y = m_offset_y - camera.y();
        if(y > camera.h() || x > camera.w() || x < (-static_cast<int>(m_width)) || y < (-static_cast<int>(m_height))) {
            ;
        }
        else {m_img.render(x, y);}
    }
    return true;
}

