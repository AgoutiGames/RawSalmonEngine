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

#include "graphics/texture.hpp"
#include "map/tile.hpp"
#include "util/tinyxml2.h"

std::string Tileset::m_base_path = "../data/";
unsigned Tileset::m_base_tile_w = 0;
unsigned Tileset::m_base_tile_h = 0;
SDL_Renderer** Tileset::mpp_renderer = nullptr;
std::vector<Tileset*> Tileset::mp_tilesets;
unsigned Tileset::m_left_overhang = 0;
unsigned Tileset::m_right_overhang = 0;
unsigned Tileset::m_up_overhang = 0;
unsigned Tileset::m_down_overhang = 0;

/**
 * @brief Constructs an empty tileset object and adds it's pointer to @c mp_tilesets
 */
Tileset::Tileset() {
    mp_tilesets.push_back(this);
    m_tileset_id = mp_tilesets.size() - 1;
}

/**
 * @brief Copies nothing, only overwrites old pointer with new one in @c mp_tilesets
 */
Tileset::Tileset(const Tileset& other) :
m_tileset_id{other.m_tileset_id} {
    mp_tilesets[m_tileset_id] = this;
}

Tileset::~Tileset() {

}

/**
 * @brief Initializes the tileset class with map info
 * @param base_tile_w, base_tile_h Size of the base tile
 * @param renderer The @c SDL_Renderer used to render to screen
 */
void Tileset::initialize(SDL_Renderer** renderer, unsigned base_tile_w, unsigned base_tile_h) {
    mp_tilesets.clear();
    mpp_renderer = renderer;
    m_base_tile_w = base_tile_w;
    m_base_tile_h = base_tile_h;
}

/**
 * @brief Initialize a tileset from XML info
 * @param ts_file The @c XMLElement which storest the tileset information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Tileset::init(tinyxml2::XMLElement* ts_file) {

    using namespace tinyxml2;

    XMLError eResult;
    eResult = ts_file->QueryUnsignedAttribute("firstgid", &m_first_gid);
    if(eResult != XML_SUCCESS) return eResult;

    // If attribute "source" is set, load external .tsx tileset file
    // Pass the new full path to the parsing!!
    // If tsx file is in a different folder, image files referenced from
    // there can't be found, because the refer to the tsx files base path
    const char* p_source;
    p_source = ts_file->Attribute("source");
    XMLDocument tsx_tileset{true, tinyxml2::COLLAPSE_WHITESPACE};
    if(p_source != nullptr) {
        std::string full_path = m_base_path + std::string(p_source);

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
    m_image.loadFromFile(*mpp_renderer, m_base_path + std::string(p_ts_source));
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

    // Set the clip rect of each tile in the tileset
    for(unsigned i_tile = 0; i_tile < m_tile_count; i_tile++) {
        SDL_Rect temp;
        temp.x = i_tile % (m_width / m_tile_width) * m_tile_width;
        temp.y = i_tile / (m_width / m_tile_width) * m_tile_height;
        temp.w = m_tile_width;
        temp.h = m_tile_height;

        // Construct each tile of the tilset and store in m_tiles
        m_tiles.push_back(Tile(this, temp));
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
                const char* p_mode;
                p_mode = p_property->Attribute("value");
                std::string mode(p_mode);
                if(p_mode == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                else if(mode == "NONE") m_image.setBlendMode(SDL_BLENDMODE_NONE);
                else if(mode == "ALPHA") m_image.setBlendMode(SDL_BLENDMODE_BLEND);
                else if(mode == "ADD") m_image.setBlendMode(SDL_BLENDMODE_ADD);
                else if(mode == "COLOR") m_image.setBlendMode(SDL_BLENDMODE_MOD);
                else {
                std::cerr << "Unknown blend mode \""<< mode << "\" specified in tileset: "<< m_name<< "\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
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

    // Tile::diagnose();
    //std::cerr << "\n\n";

    // Check if there is specific tile info
    XMLElement* p_tile = ts_file->FirstChildElement("tile");
    if(p_tile != nullptr) {

        // Static method of Tile class deals with the parsing of all tiles
        eResult = Tile::parse_from_tileset(p_tile, m_first_gid);
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
bool Tileset::render(Uint16 local_tile_id, int x, int y) const {
    bool success = true;
    if(local_tile_id >= m_tiles.size()) {
        std::cerr << "Local tileset tile id " << local_tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        m_tiles[local_tile_id].render(x, y);
    }
    return success;
}

/**
 * @brief Sets the opacity of all tileset images
 * @param opacity The float value specifying the opacity value
 */
void Tileset::set_opacity(float opacity) {
    Uint8 alpha = static_cast<Uint8>(opacity * 255);
    for(Tileset* ts : mp_tilesets) {
        ts->m_image.setAlpha(alpha);
    }
}

/**
 * @brief Write the required render margin to the static overhang variables
 *
 * Since we support oversized tiles and tileset offsets, we have to render an additional
 * margin around the actual screen to display an extra tree for example, which protrudes
 * from the lower screen boundary into the screen. This is easier than checking each tile
 * seperately while having a better performance than without offcreen tile culling.
 *
 * @todo Use pixel margin instead of tile margin for possibly slightly better performance
 */
void Tileset::write_overhang() {
    // Reset the overhang values
    m_up_overhang = 0;
    m_down_overhang = 0;
    m_left_overhang = 0;
    m_right_overhang = 0;

    // Check all registeres tileset objects
    for(Tileset* ts: mp_tilesets) {
        unsigned pix_up = 0;
        unsigned pix_down = 0;
        unsigned pix_left = 0;
        unsigned pix_right = 0;

        // Translate offset into required margin sizes
        if (ts->m_x_offset > 0) {
            pix_left += ts->m_x_offset;
        }
        else if (ts->m_x_offset < 0) {
            pix_right -= ts->m_x_offset;
        }
        if (ts->m_y_offset > 0) {
            pix_up += ts->m_y_offset;
        }
        else if(ts->m_y_offset < 0) {
            pix_down -= ts->m_y_offset;
        }

        // Take oversized tiles into account
        pix_left += ts->m_tile_width - m_base_tile_w;
        pix_down += ts->m_tile_height - m_base_tile_h;

        // Translate the pixel margin into a tile margin
        // @todo? Using the pixel values, the rendering could
        // be slightly faster if there are many oversized tiles
        // which are not divisible by the base tile size
        unsigned loc_up = pix_up / m_base_tile_h;
        if(pix_up % m_base_tile_h > 0) loc_up++;
        unsigned loc_down = pix_down / m_base_tile_h;
        if(pix_down % m_base_tile_h > 0) loc_down++;
        unsigned loc_left = pix_left / m_base_tile_w;
        if(pix_left % m_base_tile_w > 0) loc_left++;
        unsigned loc_right = pix_right / m_base_tile_w;
        if(pix_right % m_base_tile_w > 0) loc_right++;

        // Update the overhang if it's the new maximum
        if(loc_up > m_up_overhang) m_up_overhang = loc_up;
        if(loc_down > m_down_overhang) m_down_overhang = loc_down;
        if(loc_left > m_left_overhang) m_left_overhang = loc_left;
        if(loc_right > m_right_overhang) m_right_overhang = loc_right;
    }
    return;
}

SDL_Renderer* Tileset::get_renderer(){
    return *mpp_renderer;
}

const Texture* Tileset::get_image_pointer() const {
    return &m_image;
}

unsigned Tileset::get_tile_height() const {
    return m_tile_height;
}

int Tileset::get_x_offset() const {
    return m_x_offset;
}

int Tileset::get_y_offset() const {
    return m_y_offset;
}
