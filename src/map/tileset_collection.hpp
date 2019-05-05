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
#ifndef TILESET_COLLECTION_HPP_INCLUDED
#define TILESET_COLLECTION_HPP_INCLUDED

#include <vector>
#include <SDL2/SDL.h>

#include "util/tinyxml2.h"
#include "util/game_types.hpp"

class Tileset; // forward declaration
class Tile;
class MapData;

// This class SHOULD NOT be copy constructable or assignable, only MOVABLE since moving m_tilesets and their internal
// Tile objects keeps their memory adresses stable and therefore mp_tiles will still be in a valid state
class TilesetCollection{
    public:

        TilesetCollection() = default;
        TilesetCollection(TilesetCollection& tsc) = delete;
        TilesetCollection& operator= ( const TilesetCollection& ) = delete;

        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData* mapdata);

        unsigned get_tile_h() const {return m_tile_h;} ///< Return base tile height
        unsigned get_tile_w() const {return m_tile_w;} ///< Return base tile width

        unsigned get_overhang(Direction dir) const;

        Uint16 get_gid(Tile* tile)  const;
        Tile* get_tile(Uint16 tile_id) const;

        bool register_tile(Tile* tile, unsigned gid);
        void set_tile_animated(unsigned gid);
        void set_tile_animated(Tile* tile);

        void init_anim_tiles();
        void push_all_anim();

        bool render(Uint16 tile_id, int x, int y) const;
        bool render(Uint16 tile_id, SDL_Rect& dest) const;

        MapData& get_mapdata() {return *mp_base_map;}

    private:
        MapData* mp_base_map = nullptr;

        unsigned m_tile_w; // The tile dimensions in pixels
        unsigned m_tile_h;

        void write_overhang(); // sets the 4 values below v v
        unsigned m_up_overhang = 0;
        unsigned m_down_overhang = 0;
        unsigned m_left_overhang = 0;
        unsigned m_right_overhang = 0;

        std::vector<Tileset> m_tilesets; ///< Contains all used Tilesets

        std::vector<Tile*> mp_tiles;      ///< List of pointers to all tiles in order
        std::vector<Uint16> m_anim_tiles; ///< List of ids of all animated tiles
};


#endif // TILESET_COLLECTION_HPP_INCLUDED
