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
#include "map/map_layer.hpp"

#include <iostream>
#include <math.h>

#include "map/mapdata.hpp"
#include "map/layer_collection.hpp"
#include "map/tile.hpp"
#include "map/tileset_collection.hpp"
#include "util/base64.h"
#include "util/game_types.hpp"

/// Factory function which retrieves a pointer owning the map layer
MapLayer* MapLayer::parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) {
    return new MapLayer(source, name, layer_collection, eresult);
}

/// Constructor which only may be used internally
MapLayer::MapLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) : Layer(name, layer_collection) {
    eresult = init(source);
}

/**
 * @brief Initialize the layer by parsing info from source
 * @param source The @c XMLElement from which information is parsed
 * @return @c XMLError which indicates failure or sucess of parsing
 */
tinyxml2::XMLError MapLayer::init(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    m_ts_collection = &(m_layer_collection->get_base_map().get_ts_collection());

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

    // Parse actual map data
    XMLElement* p_data = source->FirstChildElement("data");
    if(p_data == nullptr) return XML_ERROR_PARSING_ELEMENT;

    // Check encoding of data
    // Only base64 encoding is supported right now
    const char* p_encoding = p_data->Attribute("encoding");
    if(p_encoding == nullptr || std::string("base64") != p_encoding) {
        std::cerr << "Encoding type: " << p_encoding << " is not supported !\n";
        std::cerr << "Use base64!\n";
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

    return XML_SUCCESS;
}

/**
 * @brief Renders the map layer to screen according to camera
 * @param camera Our active camera
 * @return @c bool which indicates sucess
 */
bool MapLayer::render(const Camera& camera) const {
    bool success = true;
    for(auto& t : clip(camera.get_rect())) {
        if(!m_ts_collection->render(std::get<0>(t),std::get<1>(t),std::get<2>(t))) {
            success = false;
        }
    }
    return success;
}

/**
 * @brief Generates a vector of tiles information for tiles possibly bounding with a rect
 * @param rect The rectangular space which the tiles are bounding with
 * @return A vector of TileId, x-coord(relative to recto origin), y-coord(relative to rect origin), tuples
 */
std::vector< std::tuple<Uint16, int, int> > MapLayer::clip(const SDL_Rect& rect) const {

    const TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();

    bool staggered = false;
    if(layout.orientation != "orthogonal") {
        staggered = true;
    }

    int base_tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int base_tile_h = static_cast<int>(m_ts_collection->get_tile_h());

    int tile_w = base_tile_w;
    int tile_h = base_tile_h;

    if(staggered) {
        if(layout.stagger_axis_y) {
            tile_h /= 2;
            tile_h += layout.hexsidelength / 2;
        }
        else {
            tile_w /= 2;
            tile_w += layout.hexsidelength / 2;
        }
    }

    // Apply the layer offset
    int x_camera = rect.x - m_offset_x;
    int y_camera = rect.y - m_offset_y;

    // Horizontal range of tiles to render
    int x_tile_from = x_camera / tile_w;
    int x_tile_to = (x_camera + rect.w) / tile_w;

    // Horizontal pixel offset to full tile
    int x_tile_offset = x_camera % tile_w;

    // Vertical range of tiles to render
    int y_tile_from = y_camera / tile_h;
    int y_tile_to = (y_camera + rect.h) / tile_h;

    // Vertical pixel offset to full tile
    int y_tile_offset = y_camera % tile_h;

    // Calculate tile offset by pixel basis
    float left_oh = m_ts_collection->get_overhang(Direction::left);
    float right_oh = m_ts_collection->get_overhang(Direction::right);
    float up_oh = m_ts_collection->get_overhang(Direction::up);
    float down_oh = m_ts_collection->get_overhang(Direction::down);

    if(staggered) {
        left_oh += base_tile_w / 2;
        up_oh += base_tile_h / 2;
    }

    left_oh = ceil(left_oh / tile_w);
    right_oh = ceil(right_oh / tile_w);
    up_oh = ceil(up_oh / tile_h);
    down_oh = ceil(down_oh / tile_h);

    // Apply the margin which makes up for oversized tiles and tileset offset
    x_tile_from -= left_oh;
    x_tile_to += right_oh;
    y_tile_from -= up_oh;
    y_tile_to += down_oh;

    // Pixel perfect position of the first upper left tile
    int x_start = -x_tile_offset - (left_oh * tile_w);
    int y_start = -y_tile_offset - (up_oh * tile_h);

    int x = x_start;
    int y = y_start;

    int x_step = tile_w;
    int y_step = tile_h;

    std::vector< std::tuple<Uint16, int, int> > tiles;
    tiles.reserve((y_tile_to - y_tile_from + 1) * (x_tile_to - x_tile_from + 1));

    // Iterates through vertical rows tile by tile
    for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {
        // Reset horizontal tile position
        x = x_start;
        if(staggered) {
            if(layout.stagger_axis_y) {
                if((!layout.stagger_index_odd && i_y_tile % 2 == 0) || (layout.stagger_index_odd && i_y_tile % 2 != 0)) {
                    x = x_start + (tile_w / 2);
                }
            }
        }
        // Skips vertical rows if position is off map/layer
        if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {

            // Iterates through horizontal rows tile by tile
            for(int i_x_tile = x_tile_from; i_x_tile <= x_tile_to; i_x_tile++) {

                // Skips horizontal rows if position is off map/layer
                if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                    // Get tile id from map layer data and draw at current position if tile_id is not 0
                    Uint16 tile_id = m_map_grid[i_y_tile][i_x_tile];
                    // Scrap empty tiles!
                    if(tile_id != 0) {
                        int temp_y = y;

                        if(staggered) {
                            if(!layout.stagger_axis_y) {
                                if((!layout.stagger_index_odd && i_x_tile % 2 == 0) || (layout.stagger_index_odd && i_x_tile % 2 != 0)) {
                                    temp_y = y + (tile_h / 2);
                                }
                            }
                        }
                        tiles.emplace_back(tile_id, x, temp_y);
                    }
                }
                // Move to next horizontal tile position
                x += x_step;
            }
        }
        // Move to next vertical tile position
        y += y_step;
    }
    return tiles;
}

