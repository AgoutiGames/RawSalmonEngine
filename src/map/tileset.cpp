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
#include "map/tileset.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "core/gameinfo.hpp"
#include "map/mapdata.hpp"
#include "map/tile.hpp"
#include "util/logger.hpp"
#include "util/parse.hpp"

using namespace salmon;
using namespace salmon::internal;

/**
 * @brief Initialize a tileset from XML info
 * @param ts_file The @c XMLElement which stores the tileset information
 * @param ts_collection Reference to tileset collection to register tiles, etc.
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Tileset::init(tinyxml2::XMLElement* ts_file, TilesetCollection& ts_collection) {

    using namespace tinyxml2;

    mp_ts_collection = &ts_collection;

    XMLError eResult;
    eResult = ts_file->QueryUnsignedAttribute("firstgid", &m_first_gid);
    if(eResult != XML_SUCCESS) return eResult;

    // If attribute "source" is set, load external .tsx tileset file
    std::string full_path = ts_collection.get_mapdata().get_file_path();

    const char* p_source;
    p_source = ts_file->Attribute("source");
    XMLDocument tsx_tileset{true, tinyxml2::COLLAPSE_WHITESPACE};
    if(p_source != nullptr) {
        full_path += std::string(p_source);

        XMLError eResult = tsx_tileset.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) return eResult;

        // Trim string
        full_path.erase(full_path.find_last_of('/') + 1);

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

    // margin and spacing are optional so it's okay to not return XML_SUCCESS
    eResult = ts_file->QueryUnsignedAttribute("margin", &m_margin);
    eResult = ts_file->QueryUnsignedAttribute("spacing", &m_spacing);

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

    const char* p_color_key;
    p_color_key = p_image->Attribute("trans");
    if (p_color_key == nullptr) {
        m_image = mp_ts_collection->get_mapdata().get_game().get_texture_cache().get(full_path + std::string(p_ts_source));
    }
    else {
        std::string color_string = p_color_key;
        SDL_Color color_key = str_to_color(color_string);
        m_image = mp_ts_collection->get_mapdata().get_game().get_texture_cache().get(full_path + std::string(p_ts_source),color_key);
    }

    if(!m_image.valid()) {
        Logger(Logger::error) << "Failed to load tileset: " << m_name << " image file: " << full_path + std::string(p_ts_source);
        return XML_ERROR_FILE_COULD_NOT_BE_OPENED;
    }
    eResult = p_image->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = p_image->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;

    // Set reserve to keep pointers to tile stable!
    m_tiles.reserve(m_tile_count);

    unsigned i_tile = 0;
    SDL_Rect temp;
    temp.w = m_tile_width;
    temp.h = m_tile_height;

    int y_advance = m_spacing + m_tile_height;
    int x_advance = m_spacing + m_tile_width;
    int y_limit = m_height - m_tile_height - m_margin;
    int x_limit = m_width - m_tile_width - m_margin;

    for(temp.y = m_margin; temp.y <= y_limit; temp.y += y_advance) {
        for(temp.x = m_margin; temp.x <= x_limit; temp.x += x_advance) {
            // Construct each tile of the tilset and store in m_tiles
            m_tiles.push_back(Tile(this, temp));
            if(!ts_collection.register_tile(&m_tiles.back(), i_tile + m_first_gid)) {
                Logger(Logger::error) << "Failed to register Tile, abort parsing process!";
                return XML_ERROR_PARSING;
            }
            i_tile++;
        }
    }

    if(i_tile != m_tile_count) {
        Logger(Logger::error) << "Wrong tile count given in .tmx/.tsx file in tileset: " << m_name;
        return XML_ERROR_PARSING;
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
                eResult = parse::blendmode(p_property, m_image);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Failed at parsing blend mode for tileset: " << m_name;
                    return eResult;
                }
            }
            else{
                Logger(Logger::error) << "Unknown tileset property " << p_name << " occured in tileset: " << m_name;
                return XML_ERROR_PARSING;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }

    // Check if there is specific tile info
    XMLElement* p_tile = ts_file->FirstChildElement("tile");

    if(p_tile != nullptr) {

        // Temporarily set base path to tileset location so the path of file attributes is right
        std::string backup = ts_collection.get_mapdata().get_file_path();
        ts_collection.get_mapdata().set_file_path(full_path);

        // Method of MapData object deals with the parsing of all tiles
        eResult = parse_tile_info(p_tile);

        // Reset file path
        ts_collection.get_mapdata().set_file_path(backup);

        if(eResult != XML_SUCCESS) {
            Logger(Logger::error) << "Failed at loading tile info of tileset: " << m_name;
            return eResult;
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Renders a tile of the tileset at a coordinate
 * @param x, y The specified coordinate
 * @param local_tile_id The ID of the tile corresponding to it's tileset
 * @return @c bool which indicates success or failure
 */
bool Tileset::render(Uint32 local_tile_id, int x, int y) const {
    bool success = true;
    if(local_tile_id >= m_tiles.size()) {
        Logger(Logger::error) << "Local tileset tile id " << local_tile_id << " is out of bounds";
        success = false;
    }
    else {
        m_tiles[local_tile_id].render(x, y);
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
    int pix_up = 0;
    int pix_down = 0;
    int pix_left = 0;
    int pix_right = 0;

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

    if (pix_up < 0) {pix_up = 0;}
    if (pix_down < 0) {pix_down = 0;}
    if (pix_left < 0) {pix_left = 0;}
    if (pix_right < 0) {pix_right = 0;}

    std::map<Direction, unsigned> oh_map;
    oh_map[Direction::up] = pix_up;
    oh_map[Direction::down] = pix_down;
    oh_map[Direction::left] = pix_left;
    oh_map[Direction::right] = pix_right;

    return oh_map;
}

/**
 * @brief Parse tile information from tileset
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tileset::parse_tile_info(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    XMLError eResult;

    while(source != nullptr) {
        unsigned tile_id;
        eResult = source->QueryUnsignedAttribute("id", &tile_id);
        if(eResult != XML_SUCCESS) return eResult;

        Tile& tile = m_tiles[tile_id];
        const char* p_type;
        p_type = source->Attribute("type");

        std::string tile_type;
        if(p_type != nullptr) tile_type = p_type;

        // Parse normal map tile
        if(p_type == nullptr) {
            eResult = tile.parse_tile(source);
        }
        // Parse Actor_Animation tile
        else if(tile_type == "ACTOR_ANIMATION") {
            eResult = tile.parse_actor_anim(source);
        }
        // Parse Actor_Template
        else /*if(tile_type == "ACTOR_TEMPLATE")*/ {
            eResult = tile.parse_actor_templ(source);
        }
        /*
        else {
            Logger(Logger::error) << "Unknown tile type: " << tile_type;
            return XML_WRONG_ATTRIBUTE_TYPE;
        }*/

        if(eResult != XML_SUCCESS) {
            Logger(Logger::error) << "Failed at loading tile gid: " << tile_id + m_first_gid << " local id: " << tile_id;
            return eResult;
        }

        source = source->NextSiblingElement();
    }
    return XML_SUCCESS;
}
