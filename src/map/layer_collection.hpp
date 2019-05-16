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
#ifndef LAYER_COLLECTION_HPP_INCLUDED
#define LAYER_COLLECTION_HPP_INCLUDED

#include <vector>
#include <SDL2/SDL.h>

#include "util/tinyxml2.h"
#include "util/game_types.hpp"

class Actor;
class Camera;
class Layer;
class MapData;

class LayerCollection {
    public:

        LayerCollection() = default;
        ~LayerCollection();

        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map);

        bool render(const Camera& camera) const;
        void update();
        std::vector<Actor*> get_actors(std::string name = "", Direction direction = Direction::invalid,
                                      AnimationType animation = AnimationType::invalid);
        bool collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type = "COLLIDE");
        bool collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type = "COLLIDE");
        bool collide(const SDL_Rect* rect, std::string type = "COLLIDE");

        MapData& get_base_map() {return *m_base_map;}

        // Don't allow copy construction and assignment because our destructor would delete twice!
        LayerCollection(const LayerCollection& other) = delete;
        LayerCollection& operator=(const LayerCollection& other) = delete;

        LayerCollection(LayerCollection&& other) = default;
        LayerCollection& operator=(LayerCollection&& other) = default;

    private:

        MapData* m_base_map;
        std::vector<Layer*> m_layers;

};


#endif // LAYER_COLLECTION_HPP_INCLUDED
