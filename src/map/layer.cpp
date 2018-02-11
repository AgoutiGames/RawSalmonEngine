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

#include <string>
#include <sstream>
#include <iostream>

#include "map/tile.hpp"
#include "util/base64.h"
#include "util/tinyxml2.h"

/**
 * Path to folder which holds the .tmx map files.
 * Possible image file paths are relative to this. */
std::string Layer::m_base_path = "../data/";

/**
 * @brief Constructs empty uninitialized Layer
 * @param tile_w, tile_h Dimensions of a standard tile
 */
Layer::Layer(unsigned tile_w, unsigned tile_h) :
m_tile_w{tile_w}, m_tile_h{tile_h}
{

}

/**
 * @brief Parse the layer information from @c XMLElement*
 * @param source The @c XMLElement* whicht stores the layer info
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Layer::init(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    XMLError eResult;

    // Parse map type "map"
    if(std::string("layer") == source->Name()) {
        m_type = map;

        // Parse layer name
        const char* p_name = source->Attribute("name");
        if (p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_name = std::string(p_name);

        // Parse layer dimensions
        eResult = source->QueryUnsignedAttribute("width", &m_width);
        if(eResult != XML_SUCCESS) return eResult;
        eResult = source->QueryUnsignedAttribute("height", &m_height);
        if(eResult != XML_SUCCESS) return eResult;

        // Parse layer offset
        int offsetx, offsety;
        eResult = source->QueryIntAttribute("offsetx", &offsetx);
        if(eResult == XML_SUCCESS) m_offset_x = offsetx;
        eResult = source->QueryIntAttribute("offsety", &offsety);
        if(eResult == XML_SUCCESS) m_offset_y = offsety;

        // Parse opacity which is currently DEFUNCT!
        float opacity;
        eResult = source->QueryFloatAttribute("opacity", &opacity);
        if(eResult == XML_SUCCESS) m_opacity = opacity;

        // Parse actual map data
        XMLElement* p_data = source->FirstChildElement("data");
        if(p_data == nullptr) return XML_ERROR_PARSING_ELEMENT;

        // Check encoding of data
        // Only base64 encoding is supported right now
        const char* p_encoding = p_data->Attribute("encoding");
        if(p_encoding == nullptr || std::string("base64") != p_encoding) {
            std::cerr << "wrong encoding type: " << p_encoding << " !\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        // Store raw map data
        std::string raw_map(p_data->GetText());

        // Decode the raw map data
        std::string data(base64_decode(raw_map));

        // Clear map from old data
        m_map_grid.resize(m_height);

        unsigned counter = 0; ///< This stores the currently read byte

        // Reassemble each tile id from 4 bytes and store in the 2d vector
        for(unsigned i_y = 0; i_y < m_height; i_y++) {
            for(unsigned i_x = 0; i_x < m_width; i_x++) {
                Uint32 tile_id = 0;
                Uint8 byte = data[counter];
                tile_id += byte;
                byte = data[counter + 1];
                tile_id += byte * 256;
                byte = data[counter + 2];
                tile_id += byte * 256 * 256;
                byte = data[counter + 3];
                tile_id += byte * 256 * 256 * 256;
                counter += 4;

                if(tile_id > 65535) {
                    std::cerr << "Tile Id is out of Uint16 Limit!\n";
                    return XML_ERROR_PARSING_TEXT;
                }

                m_map_grid[i_y].push_back(static_cast<Uint16>(tile_id));
            }
        }
    }

    // Parse map type "object"
    else if(std::string("objectgroup") == source->Name()) {
        m_type = object;

        // Parse layer name
        const char* p_name = source->Attribute("name");
        if (p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_name = std::string(p_name);

        // Parse individual objects/actors
        XMLElement* p_object = source->FirstChildElement("object");
        while(p_object != nullptr) {
            unsigned gid;
            eResult = p_object->QueryUnsignedAttribute("gid", &gid);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Object in layer: " << m_name << " has no tile id!\n";
                std::cerr << "Currently only tile objects are supported/only true actors\n";
                return eResult;
            }

            // Construct actor from its global tile id (gid)
            m_obj_grid.push_back(Actor(static_cast<Uint16>(gid)));

            // Initialize actor from the XMLElement*
            eResult = m_obj_grid[m_obj_grid.size()-1].init_actor(p_object);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at loading object in layer: " << m_name << " with gid: " << gid << "\n";
                return eResult;
            }
            // Move to next object/actor
            p_object = p_object->NextSiblingElement();
        }
    }

    // Parse map type "image"
    else if(std::string("imagelayer") == source->Name()) {
        m_type = image;

        // Parse layer name
        const char* p_name = source->Attribute("name");
        if (p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_name = std::string(p_name);

        // Parse image position
        eResult = source->QueryIntAttribute("offsetx", &m_offset_x);
        if(eResult != XML_SUCCESS) return eResult;
        eResult = source->QueryIntAttribute("offsety", &m_offset_y);
        if(eResult != XML_SUCCESS) return eResult;

        // Parse opacity (which works with image layer)
        float opacity;
        eResult = source->QueryFloatAttribute("opacity", &opacity);
        if(eResult == XML_SUCCESS) m_opacity = opacity;

        // Parse image file path
        XMLElement* p_image = source->FirstChildElement("image");
        if(p_image == nullptr) return XML_ERROR_PARSING_ELEMENT;
        const char* p_source = p_image->Attribute("source");
        if(p_source == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_img_src = m_base_path + std::string(p_source);

        // Parse the actual image data
        if(!m_img.loadFromFile(Tile::get_renderer(), m_img_src)) return XML_ERROR_PARSING;
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
                    const char* p_mode;
                    p_mode = p_property->Attribute("value");
                    std::string mode(p_mode);
                    if(p_mode == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                    else if(mode == "NONE") m_img.setBlendMode(SDL_BLENDMODE_NONE);
                    else if(mode == "ALPHA") m_img.setBlendMode(SDL_BLENDMODE_BLEND);
                    else if(mode == "ADD") m_img.setBlendMode(SDL_BLENDMODE_ADD);
                    else if(mode == "COLOR") m_img.setBlendMode(SDL_BLENDMODE_MOD);
                    else {
                    std::cerr << "Unknown blend mode specified\n";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                    }
                }
                else{
                    std::cerr << "Unknown image layer property " << p_name << " occured\n";
                    return XML_ERROR_PARSING;
                }
                // Move to next property
                p_property = p_property->NextSiblingElement("property");
            }
        }
    }

    // Return error for unknown layer types
    else {
        std::cerr << "Unknown layer type: " << source->Name() << " !\n";
        return XML_ERROR_PARSING_ELEMENT;
    }
    return XML_SUCCESS;
}

/**
 * @brief Renders the image layer to screen
 *
 * This function determines and draws the necessary tiles relative to
 * the camera position and size. This includes offscreen tile culling
 * to save processing power. A margin around the screen is also rendered
 * to make up for oversized tiles and tilset offset values.
 *
 * @param camera The rectangular area of the map to be rendered
 * @return @c bool which indicates success or failure

 * @warning The layer opacity value is ignored except for @c image type layers
 */

bool Layer::render(SDL_Rect* camera) const {
    bool success = true;

    // Render map type "map"
    if(m_type == map) {
        // Cast tile_w and h to int to simplify calculations
        int tile_w = static_cast<int>(m_tile_w);
        int tile_h = static_cast<int>(m_tile_h);

        // Apply the layer offset
        int x_camera = camera->x - m_offset_x;
        int y_camera = camera->y - m_offset_y;

        // Horizontal range of tiles to render
        int x_tile_from = x_camera / tile_w;
        int x_tile_to = (x_camera + camera->w) / tile_w;

        // Horizontal pixel offset to full tile
        int x_tile_offset = x_camera % tile_w;

        // Vertical range of tiles to render
        int y_tile_from = y_camera / tile_h;
        int y_tile_to = (y_camera + camera->h) / tile_h;

        // Vertical pixel offset to full tile
        int y_tile_offset = y_camera % tile_h;

        // Apply the margin which makes up for oversized tiles and tileset offset
        x_tile_from -= Tileset::m_left_overhang;
        x_tile_to += Tileset::m_right_overhang;
        y_tile_from -= Tileset::m_up_overhang;
        y_tile_to += Tileset::m_down_overhang;

        // Pixel perfect position of the first upper left tile
        int x = -x_tile_offset - (Tileset::m_left_overhang * m_tile_w);
        int y = -y_tile_offset - (Tileset::m_up_overhang * m_tile_h);

        // if(m_opacity < 1.0f) Tileset::set_opacity(m_opacity);

        // Iterates through vertical rows tile by tile
        for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {

            // Skips vertical rows if position is off map/layer
            if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {

                // Iterates through horizontal rows tile by tile
                for(int i_x_tile = x_tile_from; i_x_tile <= x_tile_to; i_x_tile++) {

                    // Skips horizontal rows if position is off map/layer
                    if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                        // Get tile id from map layer data and draw at current position if tile_id is not 0
                        Uint16 tile_id = m_map_grid[i_y_tile][i_x_tile];
                        if(tile_id != 0) {
                            if(!Tile::render(tile_id,x,y)) success = false;
                        }
                    }
                    // Move to next horizontal tile position
                    x += tile_w;
                }
            }
            // Reset horizontal tile position
            x = -x_tile_offset - (Tileset::m_left_overhang * m_tile_w);

            // Move to next vertical tile position
            y += tile_h;
        }
        // if(m_opacity < 1.0f) Tileset::set_opacity();
    }

    // Render map type "object"
    else if(m_type == object) {
        int x = m_offset_x - camera->x;
        int y = m_offset_y - camera->y;
        // Warning! No offscreen object culling
        for (unsigned i = 0; i < m_obj_grid.size(); i++) {
            m_obj_grid[i].render(x,y);
        }

    }

    // Render map type "image"
    else if(m_type == image) {
        int x = m_offset_x - camera->x;
        int y = m_offset_y - camera->y;
        if(y > camera->h || x > camera->w || x < (-static_cast<int>(m_width)) || y < (-static_cast<int>(m_height))) {
            return success;
        }
        else m_img.render(Tile::get_renderer(), x, y);
    }

    else {

    }

    return success;
}

/**
 * @brief Updates the states of the objects in the layer
 *
 * This function currently triggers the update function of every
 * object of an object type layer. When it returns false, this is
 * a signal to delete the object.
 */
void Layer::update() {
    if(m_type == object) {
        for (unsigned i = 0; i < m_obj_grid.size(); i++) {
            if (m_obj_grid[i].update() == false) {
                m_obj_grid.erase(m_obj_grid.begin() + i);
                i--;
            }
        }
    }

    else {

    }
}
