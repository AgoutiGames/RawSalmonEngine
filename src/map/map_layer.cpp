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
#include "map/map_layer.hpp"

#include <algorithm>
#include <iostream>
#include <math.h>
#include <sstream>
#include <zlib.h>
#include <b64/decode.h>

#include "transform.hpp"
#include "map/mapdata.hpp"
#include "map/layer_collection.hpp"
#include "map/tile.hpp"
#include "map/tileset_collection.hpp"
#include "util/game_types.hpp"
#include "util/logger.hpp"

namespace salmon { namespace internal {

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
    if(eResult != XML_SUCCESS) offsetx = 0;
    eResult = source->QueryIntAttribute("offsety", &offsety);
    if(eResult != XML_SUCCESS) offsety = 0;

    m_transform = salmon::Transform(offsetx,offsety, m_width * m_ts_collection->get_tile_w(), m_height * m_ts_collection->get_tile_h(),0,0);

    // Parse actual map data
    XMLElement* p_data = source->FirstChildElement("data");
    if(p_data == nullptr) return XML_ERROR_PARSING_ELEMENT;

    // Check encoding of data
    const char* p_encoding = p_data->Attribute("encoding");

    if(std::string("base64") == p_encoding) {

        // Store raw map data
        std::string raw_map(p_data->GetText());
        // Decode the raw map data
        base64::decoder dec;
        std::stringstream is(raw_map);
        std::stringstream os;
        dec.decode(is, os);
        const std::string& tmp = os.str();

        std::vector<Uint8> bytes(tmp.begin(), tmp.end());

        const char* p_compression = p_data->Attribute("compression");
        if(p_compression == nullptr) {;} // Skip decompress altogether
        else if(std::string("zlib") == p_compression) {
            uLongf decomp_size = m_height * m_width * 4;
            std::vector<Uint8> decomp_bytes(decomp_size);

            int result = uncompress(decomp_bytes.data(), &decomp_size, bytes.data(), bytes.size());
            if(result != Z_OK) {
                Logger(Logger::error) << "Failed decompressing zlip map data! Error code: " << result;
                return XML_ERROR_PARSING_TEXT;
            }
            bytes = decomp_bytes;
        }

        else {
            Logger(Logger::error) << "Unsupported compression " << p_compression << " for base64 encoded map";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }

        // Clear map from old data
        m_map_grid.resize(m_height);

        unsigned counter = 0; ///< This stores the currently read byte

        // Reassemble each tile id from 4 bytes and store in the 2d vector
        for(unsigned i_y = 0; i_y < m_height; i_y++) {
            for(unsigned i_x = 0; i_x < m_width; i_x++) {
                Uint32 tile_id = 0;
                Uint8 byte = bytes[counter];
                tile_id += byte;
                byte = bytes[counter + 1];
                tile_id += byte * 256;
                byte = bytes[counter + 2];
                tile_id += byte * 256 * 256;
                byte = bytes[counter + 3];
                tile_id += byte * 256 * 256 * 256;
                counter += 4;
                m_map_grid[i_y].push_back(tile_id);
            }
        }
    }

    else if(std::string("csv") == p_encoding) {
        std::stringstream ss(p_data->GetText());
        // Clear map from old data
        m_map_grid.resize(m_height);
        for(unsigned i_y = 0; i_y < m_height; i_y++) {
            for(unsigned i_x = 0; i_x < m_width; i_x++) {
                if(ss.good()){
                    std::string tile_id_str;
                    getline( ss, tile_id_str, ',' );
                    Uint32 tile_id = static_cast<Uint32>(std::stoul(tile_id_str));
                    m_map_grid[i_y].push_back(tile_id);
                }
                else {
                    Logger(Logger::error) << "Tile ids ended prematurely at x: " << i_x << " y: " << i_y;
                    return XML_ERROR_PARSING_TEXT;
                }
            }
        }
    }

    else {
        Logger(Logger::error) << "Encoding type: " << p_encoding << " is not supported !";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}

/**
 * @brief Renders the map layer to screen according to camera
 * @param camera Our active camera
 * @return @c bool which indicates sucess
 */
bool MapLayer::render(const Camera& camera) const {
    if(m_hidden) {return true;}
    bool success = true;
    for(auto& t : clip(camera.get_transform().to_rect())) {
        if(!m_ts_collection->render(std::get<0>(t),std::get<1>(t),std::get<2>(t))) {
            success = false;
        }
    }
    return success;
}

/**
 * @brief Fetch and return all tiles which are possibly bounding with the given rect
 * @param A rect which is usually a camera or the bounding box of a collider
 * @return A vector of 3-tuples consisting of the tile id and xy-coords relative to the rect origin!
 */
std::vector< std::tuple<Uint32, int, int> > MapLayer::clip(Rect rect) const {
    const MapData::TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();
    std::vector< std::tuple<Uint32, int, int> > tiles;
    if(layout.orientation == "orthogonal") {
        tiles = clip_ortho(rect);
    }
    else if(layout.stagger_axis_y) {
        tiles = clip_y_stagger(rect);
    }
    else {
        tiles = clip_x_stagger(rect);
    }
    if(layout.render_order == "left-up" || layout.render_order == "right-up") {
        std::reverse(tiles.begin(), tiles.end());
    }
    return tiles;
}

/**
 * @brief Fetch and return all tiles which are possibly bounding with the given rect
 * @param A rect which is usually the bounding box of a collider
 * @return A vector of TileInstance objects holding a pointer to the tile and xy-coords relative to the world origin!
 */
std::vector<TileInstance> MapLayer::get_clip(Rect rect) const {
    std::vector< std::tuple<Uint32, int, int> > old = clip(rect);
    std::vector<TileInstance> tiles;
    // Get missing decimals back which were eliminated due to rounding when clipping
    Point p = m_transform.get_relative(0,0);
    float x_decimals = round(rect.x - p.x) - (rect.x - p.x);
    float y_decimals = round(rect.y - p.y) - (rect.y - p.y);

    const Uint32 FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    const Uint32 FLIPPED_VERTICALLY_FLAG   = 0x40000000;
    const Uint32 FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

    for(std::tuple<Uint32, int, int> tile : old) {
        Uint32 tile_id = std::get<0>(tile);
        Tile* tile_p = m_ts_collection->get_tile(tile_id);

        Transform trans = {x_decimals + std::get<1>(tile)+rect.x,
                           y_decimals + std::get<2>(tile)+rect.y,
                           static_cast<float>(tile_p->get_w()),
                           static_cast<float>(tile_p->get_h()),
                           0,0};
        trans.set_rotation_center(0.5,0.5);
        if(tile_id >= FLIPPED_DIAGONALLY_FLAG) {
            // Read out flags
            bool flipped_horizontally = (tile_id & FLIPPED_HORIZONTALLY_FLAG);
            bool flipped_vertically = (tile_id & FLIPPED_VERTICALLY_FLAG);
            bool flipped_diagonally = (tile_id & FLIPPED_DIAGONALLY_FLAG);
            double angle = 0;
            // This snippet was determined via trial and error
            // I have no idea why this even works, but it does
            if(flipped_diagonally) {
                angle = 270;
                if(flipped_horizontally == flipped_vertically) {
                    angle = 90;
                }
                flipped_vertically = !flipped_vertically;
            }
            trans.set_h_flip(flipped_horizontally);
            trans.set_v_flip(flipped_vertically);
            trans.set_rotation(angle);
        }

        tiles.push_back({tile_p,trans});
    }
    return tiles;
}

/**
 * @brief Generates a vector of tiles information for tiles possibly bounding with a rect
 * @param rect The rectangular space which the tiles are bounding with
 * @return A vector of TileId, x-coord(relative to recto origin), y-coord(relative to rect origin), tuples
 */
std::vector< std::tuple<Uint32, int, int> > MapLayer::clip_ortho(Rect rect) const {

    const MapData::TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();

    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());

    int x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start;
    calc_tile_range(rect, tile_w, tile_h, x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start);

    std::vector< std::tuple<Uint32, int, int> > tiles;
    tiles.reserve((y_tile_to - y_tile_from + 1) * (x_tile_to - x_tile_from + 1));

    int y = y_start;
    // Iterates through vertical rows tile by tile
    for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {
        // Reset horizontal tile position
        int x = x_start;
        // Skips vertical rows if position is off map/layer
        if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {

            std::vector< std::tuple<Uint32, int, int> > x_tiles;
            x_tiles.reserve(x_tile_to - x_tile_from + 1);

            // Iterates through horizontal rows tile by tile
            for(int i_x_tile = x_tile_from; i_x_tile <= x_tile_to; i_x_tile++) {

                // Skips horizontal rows if position is off map/layer
                if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                    // Get tile id from map layer data and draw at current position if tile_id is not 0
                    Uint32 tile_id = m_map_grid[i_y_tile][i_x_tile];
                    // Scrap empty tiles!
                    if(tile_id != 0) {
                        x_tiles.emplace_back(tile_id, x, y);
                    }
                }
                // Move to next horizontal tile position
                x += tile_w;
            }

            if(layout.render_order == "left-down" || layout.render_order == "right-up") {
                std::reverse(x_tiles.begin(), x_tiles.end());
            }
            tiles.insert(tiles.end(),x_tiles.begin(),x_tiles.end());

        }
        // Move to next vertical tile position
        y += tile_h;
    }
    return tiles;
}

/**
 * @brief Generates a vector of tiles information for tiles possibly bounding with a rect
 * @param rect The rectangular space which the tiles are bounding with
 * @return A vector of TileId, x-coord(relative to rect origin), y-coord(relative to rect origin), tuples
 */
std::vector< std::tuple<Uint32, int, int> > MapLayer::clip_y_stagger(Rect rect) const {

    const MapData::TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();

    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());

    // Conform to y_stagger
    tile_h /= 2;
    tile_h += layout.hexsidelength / 2;

    int x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start;
    calc_tile_range(rect, tile_w, tile_h, x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start);

    std::vector< std::tuple<Uint32, int, int> > tiles;
    tiles.reserve((y_tile_to - y_tile_from + 1) * (x_tile_to - x_tile_from + 1));

    int y = y_start;
    // Iterates through vertical rows tile by tile
    for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {
        // Reset horizontal tile position
        int x = x_start;
        if((!layout.stagger_index_odd && i_y_tile % 2 == 0) || (layout.stagger_index_odd && i_y_tile % 2 != 0)) {
            x = x_start + (tile_w / 2);
        }

        // Skips vertical rows if position is off map/layer
        if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {

            std::vector< std::tuple<Uint32, int, int> > x_tiles;
            x_tiles.reserve(x_tile_to - x_tile_from + 1);

            // Iterates through horizontal rows tile by tile
            for(int i_x_tile = x_tile_from; i_x_tile <= x_tile_to; i_x_tile++) {

                // Skips horizontal rows if position is off map/layer
                if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                    // Get tile id from map layer data and draw at current position if tile_id is not 0
                    Uint32 tile_id = m_map_grid[i_y_tile][i_x_tile];
                    // Scrap empty tiles!
                    if(tile_id != 0) {
                        x_tiles.emplace_back(tile_id, x, y);
                    }
                }
                // Move to next horizontal tile position
                x += tile_w;
            }

            if(layout.render_order == "left-down" || layout.render_order == "right-up") {
                std::reverse(x_tiles.begin(), x_tiles.end());
            }
            tiles.insert(tiles.end(),x_tiles.begin(),x_tiles.end());

        }
        // Move to next vertical tile position
        y += tile_h;
    }
    return tiles;
}

/**
 * @brief Generates a vector of tiles information for tiles possibly bounding with a rect
 * @param rect The rectangular space which the tiles are bounding with
 * @return A vector of TileId, x-coord(relative to recto origin), y-coord(relative to rect origin), tuples
 */
std::vector< std::tuple<Uint32, int, int> > MapLayer::clip_x_stagger(Rect rect) const {

    const MapData::TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();

    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());

    // Conform to x stagger
    tile_w /= 2;
    tile_w += layout.hexsidelength / 2;

    int x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start;
    calc_tile_range(rect, tile_w, tile_h, x_tile_from, x_tile_to, y_tile_from, y_tile_to, x_start, y_start);

    // Per row fetch every second tile
    int x_step = tile_w * 2;
    // Split each row into two
    int y_step = tile_h / 2;

    std::vector< std::tuple<Uint32, int, int> > tiles;
    tiles.reserve((y_tile_to - y_tile_from + 1) * (x_tile_to - x_tile_from + 1));

    // Determine if first half row starts odd or even
    int odd_even = 0;
    if((!layout.stagger_index_odd && x_tile_from % 2 == 0) || (layout.stagger_index_odd && x_tile_from % 2 != 0)) {
        odd_even = 1;
    }

    int y = y_start;
    // Iterates through vertical rows tile by tile
    for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {
        // Skips vertical rows if position is off map/layer
        if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {
            // Fetch row in two passes for correct rendering order
            for(int i_odd_even = 0; i_odd_even < 2; i_odd_even++) {

                std::vector< std::tuple<Uint32, int, int> > x_tiles;
                x_tiles.reserve((x_tile_to - x_tile_from + 1) / 2);

                // Reset horizontal tile position
                int x = x_start;
                int x_offset = (i_odd_even + odd_even) % 2;
                x += x_offset * tile_w;

                // Iterates through horizontal rows tile by tile
                for(int i_x_tile = x_tile_from + x_offset; i_x_tile <= x_tile_to; i_x_tile += 2) {

                    // Skips horizontal rows if position is off map/layer
                    if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                        // Get tile id from map layer data and draw at current position if tile_id is not 0
                        Uint32 tile_id = m_map_grid[i_y_tile][i_x_tile];
                        // Scrap empty tiles!
                        if(tile_id != 0) {
                            x_tiles.emplace_back(tile_id, x, y);
                        }
                    }
                    // Move to next horizontal tile position
                    x += x_step;
                }

                if(layout.render_order == "left-down" || layout.render_order == "right-up") {
                    std::reverse(x_tiles.begin(), x_tiles.end());
                }
                tiles.insert(tiles.end(),x_tiles.begin(),x_tiles.end());

                // Move down half a row
                y += y_step;
            }
            // Prevent stepping down twice
            continue;
        }
        // Move down a full row
        y += y_step * 2;
    }
    return tiles;
}

/// Calculate the range of tiles bounding with rect
void MapLayer::calc_tile_range(Rect src_rect, int tile_w, int tile_h, int& x_from, int& x_to, int& y_from, int& y_to, int& x_start, int& y_start) const {

    // Apply the layer offset
    Point p = m_transform.get_relative(0,0);
    src_rect.x -= p.x;
    src_rect.y -= p.y;

    PixelRect rect = src_rect;

    // Horizontal range of tiles to render
    x_from = rect.x / tile_w;
    x_to = (rect.x + rect.w) / tile_w;

    // Horizontal pixel offset to full tile
    int x_tile_offset = rect.x % tile_w;

    // Vertical range of tiles to render
    y_from = rect.y / tile_h;
    y_to = (rect.y + rect.h) / tile_h;

    // Vertical pixel offset to full tile
    int y_tile_offset = rect.y % tile_h;

    // Calculate tile offset by pixel basis
    float left_oh = m_ts_collection->get_overhang(Direction::left);
    float right_oh = m_ts_collection->get_overhang(Direction::right);
    float up_oh = m_ts_collection->get_overhang(Direction::up);
    float down_oh = m_ts_collection->get_overhang(Direction::down);

    const MapData::TileLayout layout = m_layer_collection->get_base_map().get_tile_layout();
    if(layout.orientation != "orthogonal") {
        // Render half tile extra to show pointy tile borders
        left_oh += static_cast<int>(m_ts_collection->get_tile_w()) / 2;
        up_oh += static_cast<int>(m_ts_collection->get_tile_h()) / 2;
    }

    left_oh = ceil(left_oh / tile_w);
    right_oh = ceil(right_oh / tile_w);
    up_oh = ceil(up_oh / tile_h);
    down_oh = ceil(down_oh / tile_h);

    // Apply the margin which makes up for oversized tiles and tileset offset
    x_from -= left_oh;
    x_to += right_oh;
    y_from -= up_oh;
    y_to += down_oh;

    // Pixel perfect position of the first upper left tile
    x_start = -x_tile_offset - (left_oh * tile_w);
    y_start = -y_tile_offset - (up_oh * tile_h);

}

}} // namespace salmon::internal
