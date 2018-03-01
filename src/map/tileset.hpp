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
#ifndef TILESET_HPP_INCLUDED
#define TILESET_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <map>

#include "graphics/texture.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

// forward-declare the parts you need in one of the files and leave the #include out of that file.

class Tile; // forward declaration
class MapData;

/**
 * @brief Parse, store and manage all tilesets
 *
 * @warning Because m_image can't be copied, initialized tilesets can't be copied either
 * @warning Tiled's "Collection Tileset" are not supported
 */

class Tileset{

    public:
        Tileset(); // Automatically appends to static tileset list
        ~Tileset();

        tinyxml2::XMLError init(tinyxml2::XMLElement* ts_file, MapData& base_map); // Initialize single object

        const Texture* get_image_pointer() const {return &m_image;}
        unsigned get_tile_height() const {return m_tile_height;}
        int get_x_offset() const {return m_x_offset;}
        int get_y_offset() const {return m_y_offset;}

        bool render(Uint16 local_tile_id, int x, int y, const MapData& base_map) const;

        std::map<Direction, unsigned> determine_overhang(unsigned tile_w, unsigned tile_h) const;

    private:
        std::string m_name;
        Texture m_image;        // The actual image file
        unsigned m_first_gid;
        unsigned m_tile_width;  // Local tile dimensions can differ from the standard ones
        unsigned m_tile_height; // but if the maximum difference gets too big, the perfomance can decrease
        unsigned m_tile_count;
        unsigned m_width;       // Dimensions of the whole tileset image
        unsigned m_height;

        // Setting these values too high can severely impact performance
        int m_x_offset = 0;
        int m_y_offset = 0;

        // Here are the actual tiles corresponding to the tileset stored
        std::vector<Tile> m_tiles;
};



#endif // TILESET_HPP_INCLUDED
