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
#include "map/tileset_collection.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "map/tile.hpp"
#include "map/mapdata.hpp"
#include "util/parse.hpp"

/**
 * @brief Initialize a TilesetCollection from XML info
 * @param source The @c XMLElement which stores the tileset information
 * @param mapdata Pointer to map to which these tilesets belong
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError TilesetCollection::init(tinyxml2::XMLElement* source, MapData* mapdata) {
    using namespace tinyxml2;

    mp_base_map = mapdata;

    XMLError eResult = source->QueryUnsignedAttribute("tilewidth", &m_tile_w);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Tile width: " << m_tile_w << "\n";
    eResult = source->QueryUnsignedAttribute("tileheight", &m_tile_h);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Tile height: " << m_tile_h << "\n";

    // All tilesets get parsed
    std::vector<XMLElement*> p_tilesets;

    XMLElement* pTs = source->FirstChildElement("tileset");
    if (pTs == nullptr) {
        std::cout << "Error: Parsing Mapfile without any Tileset!\n";
        return XML_ERROR_PARSING_ELEMENT;
        }

    // Collect all tilesets to a vector of pointers
    do{
        p_tilesets.push_back(pTs);
        pTs = pTs->NextSiblingElement("tileset");
    }while(pTs != nullptr );
    std::cout << "Tileset files: " << p_tilesets.size() << "\n";

    // Clear Tile and Tileset data
    mp_tiles.clear();
    // Since tiled adresses tiles beginning with index one, we push a nullptr to position 0
    // to avoid unnecessary addition on tile acess
    //
    // Anybody who dares manually adressing tile 0, deserves the segmentation fault ;-)
    mp_tiles.push_back(nullptr);
    m_anim_tiles.clear();

    m_tilesets.clear();
    m_tilesets.resize(p_tilesets.size());

    // Actually parse each tileset of the vector of pointers
    for(unsigned i = 0; i < p_tilesets.size(); i++) {
        eResult = m_tilesets[i].init(p_tilesets[i], *this);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing Tileset: " << i << "\n";
            return eResult;
        }
    }

    // This must be called after the parsing of all tilesets!
    // It calculates the minimum rendering overhang due to big tiles and tileset offsets
    write_overhang();

    // This must be called after the parsing of all tilesets!
    // It sets all animated tiles to their starting positions
    // and passes the current timestamp
    init_anim_tiles();

    return XML_SUCCESS;
}

/// Returns tile overhang values for the specified direction (up, down, left, right)
unsigned TilesetCollection::get_overhang(Direction dir) const{
    switch(dir) {
        case Direction::up:
            return m_up_overhang;
            break;
        case Direction::down:
            return m_down_overhang;
            break;
        case Direction::left:
            return m_left_overhang;
            break;
        case Direction::right:
            return m_right_overhang;
            break;
        default:
            std::cout << "Invalid overhang value requested!\n";
            return 0;
            break;
    }
}

/// Return global Id of a tile if it's registered or 0 if not
Uint16 TilesetCollection::get_gid(Tile* tile)  const{
    for(Uint16 i = 0; i < mp_tiles.size(); i++) {
        if(mp_tiles[i] == tile) {
            return i;
        }
    }
    std::cerr << "Could not find Tile to get its gid, not in global list!\n";
    return 0;
}

/// Returns the pointer to a tile from it's tile id
Tile* TilesetCollection::get_tile(Uint16 tile_id) const{
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        return nullptr;
    }
    else return mp_tiles[tile_id];
}

/// Registers tile so it's renderable by its gid
bool TilesetCollection::register_tile(Tile* tile, unsigned gid) {
    mp_tiles.push_back(tile);
    if(mp_tiles.size() != gid + 1) {
        std::cerr << "Global tile id does not match! Is: " << gid << " should be: " << mp_tiles.size() - 1 << "\n";
        return false;
    }
    return true;
}


/// Adds tile to the "always animate" list
void TilesetCollection::set_tile_animated(unsigned gid) {
    m_anim_tiles.push_back(gid);
}

/// Adds tile to the "always animate" list
void TilesetCollection::set_tile_animated(Tile* tile) {
    for(unsigned i = 0; i < mp_tiles.size(); i++) {
        if(mp_tiles[i] == tile) {
            m_anim_tiles.push_back(i);
            return;
        }
    }
    std::cerr << "Could not find Tile to set it to animated, not in global tile list! (has no gid)\n";
}

/// Initializes all registered animated tiles to the current timestamp and first frame
void TilesetCollection::init_anim_tiles() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        mp_tiles[tile]->init_anim(time);
    }
}

/**
 * @brief Animates all tiles
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 * Passing of time is to ensure synchronity of tile animation
 */
void TilesetCollection::push_all_anim() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        mp_tiles[tile]->push_anim(time);
    }
}


/// Checks for minimum of overhang values for each tileset and saves the corresponding maximum
void TilesetCollection::write_overhang() {
    std::map<Direction, unsigned> oh;
    // Check each tileset for maximum overhang values for each direction
    for(Tileset& ts : m_tilesets) {
        std::map<Direction, unsigned> temp;
        temp = ts.determine_overhang(m_tile_w, m_tile_h);
        // Check and write possible new maximum
        if(temp[Direction::up] > oh[Direction::up]) {oh[Direction::up] = temp[Direction::up];}
        if(temp[Direction::down] > oh[Direction::down]) {oh[Direction::down] = temp[Direction::down];}
        if(temp[Direction::left] > oh[Direction::left]) {oh[Direction::left] = temp[Direction::left];}
        if(temp[Direction::right] > oh[Direction::right]) {oh[Direction::right] = temp[Direction::right];}
    }
    // Write the final maximums to the members
    m_up_overhang = oh[Direction::up];
    m_down_overhang = oh[Direction::down];
    m_left_overhang = oh[Direction::left];
    m_right_overhang = oh[Direction::right];
}

/**
 * @brief Renders a tile by it's id at a coordinate
 * @param x, y The rendering coordinates
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid
 */
bool TilesetCollection::render(Uint16 tile_id, int x, int y) const{
    bool success = true;

    // Check if id is valid
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(x,y);
    }
    return success;
}

/**
 * @brief Renders a tile by it's id to a rect
 * @param dest The rendering rectangle
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid.
 * This function can resize the tile image
 */
bool TilesetCollection::render(Uint16 tile_id, SDL_Rect& dest) const{
    bool success = true;
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(dest);
    }
    return success;
}

