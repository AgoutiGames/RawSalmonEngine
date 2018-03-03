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
#include "map/tileset.hpp"

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "graphics/texture.hpp"
#include "map/mapdata.hpp"
#include "map/tile.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

/**
 * @brief Constructs an empty tileset object
 */
Tileset::Tileset() {

}

Tileset::~Tileset() {

}

/**
 * @brief Initialize a tileset from XML info
 * @param ts_file The @c XMLElement which storest the tileset information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Tileset::init(tinyxml2::XMLElement* ts_file, MapData& base_map) {

    using namespace tinyxml2;

    XMLError eResult;
    eResult = ts_file->QueryUnsignedAttribute("firstgid", &m_first_gid);
    if(eResult != XML_SUCCESS) return eResult;

    // If attribute "source" is set, load external .tsx tileset file
    std::string full_path = base_map.get_file_path();

    const char* p_source;
    p_source = ts_file->Attribute("source");
    XMLDocument tsx_tileset{true, tinyxml2::COLLAPSE_WHITESPACE};
    if(p_source != nullptr) {
        full_path += std::string(p_source);

        XMLError eResult = tsx_tileset.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) return eResult;

        XMLElement* pTileset = tsx_tileset.FirstChildElement("tileset");
        if (pTileset == nullptr) return XML_ERROR_PARSING_ELEMENT;

        ts_file = pTileset;
    }

    // Parse tileset name, tile dimensions and tile count
    const char* p_ts_name;
    p_ts_name = ts_file->Attribute("name");
    if(p_ts_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_name = std::string(p_ts_name);
    eResult = ts_file->QueryUnsignedAttribute("tilewidth", &m_tile_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = ts_file->QueryUnsignedAttribute("tileheight", &m_tile_height);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = ts_file->QueryUnsignedAttribute("tilecount", &m_tile_count);
    if(eResult != XML_SUCCESS) return eResult;

    // Parse tile offset values
    XMLElement* p_offset = ts_file->FirstChildElement("tileoffset");
    if(p_offset != nullptr) {
        eResult = p_offset->QueryIntAttribute("x", &m_x_offset);
        if(eResult != XML_SUCCESS) return eResult;
        eResult = p_offset->QueryIntAttribute("y", &m_y_offset);
        if(eResult != XML_SUCCESS) return eResult;
    }

    // Parse image path and dimensions, and load the image file
    XMLElement* p_image = ts_file->FirstChildElement("image");
    if(p_image == nullptr) return XML_ERROR_PARSING_ELEMENT;
    const char* p_ts_source;
    p_ts_source = p_image->Attribute("source");
    if (p_ts_source == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_image.loadFromFile(base_map.get_renderer(), full_path + std::string(p_ts_source));
    eResult = p_image->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = p_image->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;

    // Check for matching image dimensions
    // If tiles don't perfectly fit, the tileset gets rejected
    if(m_width % m_tile_width != 0) {
        std::cerr << "Image width isn't divisible by its tile width in tileset: "<< m_name<< "\n";
        return XML_ERROR_PARSING;
    }
    if(m_height % m_tile_height != 0) {
        std::cerr << "Image height isn't divisible by its tile height in tileset: "<< m_name<< "\n";
        return XML_ERROR_PARSING;
    }
    if(m_tile_count != (m_width / m_tile_width) * (m_height / m_tile_height)) {
        std::cerr << "Wrong tile count given in .tmx/.tsx file in tileset: "<< m_name<< "\n";
        return XML_ERROR_PARSING;
    }

    // Set reserve to keep pointers to tile stable!
    m_tiles.reserve(m_tile_count);

    for(unsigned i_tile = 0; i_tile < m_tile_count; i_tile++) {
        // Set the clip rect of each tile in the tileset
        SDL_Rect temp;
        temp.x = i_tile % (m_width / m_tile_width) * m_tile_width;
        temp.y = i_tile / (m_width / m_tile_width) * m_tile_height;
        temp.w = m_tile_width;
        temp.h = m_tile_height;

        // Construct each tile of the tilset and store in m_tiles
        m_tiles.push_back(Tile(this, temp));
        if(!base_map.register_tile(&m_tiles.back(), i_tile + m_first_gid)) {
            std::cerr << "Failed to register Tile, abort parsing process!\n";
            return XML_ERROR_PARSING;
        }
    }

    // Parse user specified properties of the tileset (only blend mode right now)
    XMLElement* p_properties = ts_file->FirstChildElement("properties");
    if(p_properties != nullptr) {
        XMLElement* p_property = p_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            if(name == "BLEND_MODE") {
                eResult = parse_blendmode(p_property, m_image);
                if(eResult != XML_SUCCESS) {
                    std::cerr << "Failed at parsing blend mode for tileset: " << m_name << "\n";
                    return eResult;
                }
            }
            else{
                std::cerr << "Unknown tileset property " << p_name << " occured in tileset: "<< m_name<< "\n";
                return XML_ERROR_PARSING;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }

    // Check if there is specific tile info
    XMLElement* p_tile = ts_file->FirstChildElement("tile");
    if(p_tile != nullptr) {

        // Method of MapData object deals with the parsing of all tiles
        eResult = base_map.parse_tiles_from_tileset(p_tile, m_first_gid);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading tile info of tileset: " << m_name << " \n";
            return eResult;
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Renders a tile of the tileset at a coordinate
 * @param x, y, The specified coordinate
 * @param local_tile_id The ID of the tile corresponding to it's tileset
 * @return @c bool which indicates success or failure
 */
bool Tileset::render(Uint16 local_tile_id, int x, int y, const MapData& base_map) const {
    bool success = true;
    if(local_tile_id >= m_tiles.size()) {
        std::cerr << "Local tileset tile id " << local_tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        m_tiles[local_tile_id].render(x, y, base_map);
    }
    return success;
}

/**
 * @brief Return the required render margin for this tileset
 *
 * Since we support oversized tiles and tileset offsets, we have to render an additional
 * margin around the actual screen to display an extra tree for example, which protrudes
 * from the lower screen boundary into the screen. This is easier than checking each tile
 * seperately while having a better performance than without offcreen tile culling.
 *
 * @todo Use pixel margin instead of tile margin for possibly slightly better performance
 */
std::map<Direction, unsigned> Tileset::determine_overhang(unsigned tile_w, unsigned tile_h) const{
    unsigned pix_up = 0;
    unsigned pix_down = 0;
    unsigned pix_left = 0;
    unsigned pix_right = 0;

    // Translate offset into required margin sizes
    if (m_x_offset > 0) {
        pix_left += m_x_offset;
    }
    else if (m_x_offset < 0) {
        pix_right -= m_x_offset;
    }
    if (m_y_offset > 0) {
        pix_up += m_y_offset;
    }
    else if(m_y_offset < 0) {
        pix_down -= m_y_offset;
    }

    // Take oversized tiles into account
    pix_left += m_tile_width - tile_w;
    pix_down += m_tile_height - tile_h;

    // Translate the pixel margin into a tile margin
    // @todo? Using the pixel values, the rendering could
    // be slightly faster if there are many oversized tiles
    // which are not divisible by the base tile size
    unsigned loc_up = pix_up / tile_h;
    if(pix_up % tile_h > 0) loc_up++;
    unsigned loc_down = pix_down / tile_h;
    if(pix_down % tile_h > 0) loc_down++;
    unsigned loc_left = pix_left / tile_w;
    if(pix_left % tile_w > 0) loc_left++;
    unsigned loc_right = pix_right / tile_w;
    if(pix_right % tile_w > 0) loc_right++;

    std::map<Direction, unsigned> oh_map;
    oh_map[Direction::up] = loc_up;
    oh_map[Direction::down] = loc_down;
    oh_map[Direction::left] = loc_left;
    oh_map[Direction::right] = loc_right;

    return oh_map;
}
