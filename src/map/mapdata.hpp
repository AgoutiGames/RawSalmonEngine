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
#ifndef MAPDATA_HPP_INCLUDED
#define MAPDATA_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "map/layer.hpp"
#include "map/tileset.hpp"
#include "util/tinyxml2.h"

/**
 * @brief Contain, render and update all game map layers
 *
 * This class is the starting point of parsing the Tiled mapfile.
 * It parses the used tilesets and map layers and stores them.
 * It mostly passes the instructions to the corresponding classes.
 */

class MapData {
    public:
        MapData();
        ~MapData();

        tinyxml2::XMLError init_map(std::string filename, SDL_Renderer** renderer);
        bool render(SDL_Rect* camera);
        void update();

    private:
        tinyxml2::XMLDocument m_mapfile{true, tinyxml2::COLLAPSE_WHITESPACE}; ///< Contains the .tmx map file
        std::string m_base_path = "../data/"; ///< Path to folder where .tmx map files are
        unsigned m_tile_w; // The tile dimensions
        unsigned m_tile_h;
        unsigned m_width;  // The map dimensions
        unsigned m_height;

        std::vector<Tileset> m_tilesets; ///< Contains all used Tilesets
        std::vector<Layer> m_layers; ///< Contains all used layers
};


#endif // MAPDATA_HPP_INCLUDED
