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

#ifndef MAP_LAYER_HPP_INCLUDED
#define MAP_LAYER_HPP_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <SDL2/SDL.h>

#include "map/layer.hpp"

class TilesetCollection;

class MapLayer : public Layer{
    public:

        bool render(const Camera& camera) const override;
        void update() override;

        bool collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type = "COLLIDE") override;
        bool collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type = "COLLIDE") override;
        bool collide(const SDL_Rect* rect, std::string type = "COLLIDE") override;

        LayerType get_type() override {return LayerType::map;}

        static MapLayer* parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

        MapLayer(const MapLayer& other) = delete;
        MapLayer& operator=(const MapLayer& other) = delete;

        MapLayer(MapLayer&& other) = default;
        MapLayer& operator=(MapLayer&& other) = default;

    protected:
        MapLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

    private:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        // members for m_type map
        TilesetCollection* m_ts_collection;
        unsigned m_width;   // Measured in tiles for "map" and pixels for "image"
        unsigned m_height;

        std::vector<std::vector<Uint16> > m_map_grid; ///< The actual map layer information
};


#endif // MAP_LAYER_HPP_INCLUDED
