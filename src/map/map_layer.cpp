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

#include "map/mapdata.hpp"
#include "map/layer_collection.hpp"
#include "map/tileset_collection.hpp"
#include "util/base64.h"

MapLayer* MapLayer::parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) {
    return new MapLayer(source, name, layer_collection, eresult);
}

MapLayer::MapLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) : Layer(name, layer_collection) {
    eresult = init(source);
}

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

    return XML_SUCCESS;
}

bool MapLayer::render(const Camera& camera) const {
    bool success = true;
    // Cast tile_w and h to int to simplify calculations
    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());

    // Apply the layer offset
    int x_camera = camera.x() - m_offset_x;
    int y_camera = camera.y() - m_offset_y;

    // Horizontal range of tiles to render
    int x_tile_from = x_camera / tile_w;
    int x_tile_to = (x_camera + camera.w()) / tile_w;

    // Horizontal pixel offset to full tile
    int x_tile_offset = x_camera % tile_w;

    // Vertical range of tiles to render
    int y_tile_from = y_camera / tile_h;
    int y_tile_to = (y_camera + camera.h()) / tile_h;

    // Vertical pixel offset to full tile
    int y_tile_offset = y_camera % tile_h;

    // Apply the margin which makes up for oversized tiles and tileset offset
    x_tile_from -= m_ts_collection->get_overhang(Direction::left);
    x_tile_to += m_ts_collection->get_overhang(Direction::right);
    y_tile_from -= m_ts_collection->get_overhang(Direction::up);
    y_tile_to += m_ts_collection->get_overhang(Direction::down);

    // Pixel perfect position of the first upper left tile
    int x = -x_tile_offset - (m_ts_collection->get_overhang(Direction::left) * tile_w);
    int y = -y_tile_offset - (m_ts_collection->get_overhang(Direction::up) * tile_h);

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
                        if(!m_ts_collection->render(tile_id,x,y)) success = false;
                    }
                }
                // Move to next horizontal tile position
                x += tile_w;
            }
        }
        // Reset horizontal tile position
        x = -x_tile_offset - (m_ts_collection->get_overhang(Direction::left) * tile_w);

        // Move to next vertical tile position
        y += tile_h;
    }
    return success;
}

void MapLayer::update() {

}

bool MapLayer::collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type) {
    bool collide = false;
    if(type != "COLLIDE") {return collide;}
    // Cast tile_w and h to int to simplify calculations
    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());
    // Calculate possible chunk of tiles which could possibly collide with the rect
    int x_from = (rect->x - m_offset_x) / tile_w;
    int y_from = (rect->y - m_offset_y) / tile_h;
    int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
    int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
    // Iterate through all possible tiles
    for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
        for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
            Uint16 tile_id = m_map_grid[y][x];
            // Exclude invalid tiles from check
            if(tile_id != 0) {
                Tile* tile = m_ts_collection->get_tile(tile_id);
                SDL_Rect tile_rect = tile->get_hitbox();
                // Only check collision for tiles with valid hitbox
                if(!SDL_RectEmpty(&tile_rect)) {
                    // Move tile hitbox to tile coordinates
                    tile_rect.x += m_offset_x + x * tile_w;
                    tile_rect.y += m_offset_y + y * tile_h;
                    SDL_Rect intersect;
                    // Get intersection from supplied rect and tile rect
                    if(SDL_IntersectRect(rect, &tile_rect, &intersect)) {
                        // Possibly overwrite maximum collision depth value
                        if(intersect.w > x_max) {x_max = intersect.w;}
                        if(intersect.h > y_max) {y_max = intersect.h;}
                        /*
                        std::cerr << "check " << tile_rect.x << " " << tile_rect.y << " " << tile_rect.w << " " << tile_rect.h << "\n";
                        std::cerr << "check " << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << "\n";
                        std::cerr << "x depth: " << intersect.w << "\n";
                        std::cerr << "y_depth: " << intersect.h << "\n";
                        */
                        collide = true;
                    }
                }
            }
        }
    }
    return collide;
}
bool MapLayer::collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type) {
    bool collide = false;
    if(type != "COLLIDE") {return collide;}
    // Cast tile_w and h to int to simplify calculations
    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());
    // Calculate possible chunk of tiles which could possibly collide with the rect
    int x_from = (rect->x - m_offset_x) / tile_w;
    int y_from = (rect->y - m_offset_y) / tile_h;
    int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
    int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
    // Iterate through all possible tiles
    for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
        for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
            Uint16 tile_id = m_map_grid[y][x];
            // Exclude invalid tiles from check
            if(tile_id != 0) {
                Tile* tile = m_ts_collection->get_tile(tile_id);
                SDL_Rect tile_rect = tile->get_hitbox();
                // Only check collision for tiles with valid hitbox
                if(!SDL_RectEmpty(&tile_rect)) {
                    // Move tile hitbox to tile coordinates
                    tile_rect.x += m_offset_x + x * tile_w;
                    tile_rect.y += m_offset_y + y * tile_h;
                    // Get intersection from supplied rect and tile rect
                    if(SDL_HasIntersection(rect, &tile_rect)) {
                        collide = true;
                    }
                }
            }
        }
    }
    return collide;
}
bool MapLayer::collide(const SDL_Rect* rect, std::string type) {
    bool collide = false;
    if(type != "COLLIDE") {return collide;}
    // Cast tile_w and h to int to simplify calculations
    int tile_w = static_cast<int>(m_ts_collection->get_tile_w());
    int tile_h = static_cast<int>(m_ts_collection->get_tile_h());
    // Calculate possible chunk of tiles which could possibly collide with the rect
    int x_from = (rect->x - m_offset_x) / tile_w;
    int y_from = (rect->y - m_offset_y) / tile_h;
    int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
    int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
    // Iterate through all possible tiles
    for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
        for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
            Uint16 tile_id = m_map_grid[y][x];
            // Exclude invalid tiles from check
            if(tile_id != 0) {
                Tile* tile = m_ts_collection->get_tile(tile_id);
                SDL_Rect tile_rect = tile->get_hitbox();
                // Only check collision for tiles with valid hitbox
                if(!SDL_RectEmpty(&tile_rect)) {
                    // Move tile hitbox to tile coordinates
                    tile_rect.x += m_offset_x + x * tile_w;
                    tile_rect.y += m_offset_y + y * tile_h;
                    // Get intersection from supplied rect and tile rect
                    if(SDL_HasIntersection(rect, &tile_rect)) {
                        collide = true;
                    }
                }
            }
        }
    }
    return collide;
}