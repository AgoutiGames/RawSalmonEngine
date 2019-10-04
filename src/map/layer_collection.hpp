/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#include <memory>
#include <SDL.h>
#include <tinyxml2.h>

#include "util/game_types.hpp"

class Actor;
class Camera;
class Layer;
class MapData;
class MapLayer;
class ObjectLayer;
class ImageLayer;

/**
 * @brief Container for all possible map layers. Inits, updates, draws and deletes layers.
 */
class LayerCollection {
    public:
        LayerCollection() = default;

        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map);

        bool render(const Camera& camera) const;
        void update(bool late = false);

        std::vector<Actor*> get_actors();
        std::vector<Actor*> get_actors(std::string name);
        Actor* get_actor(std::string name);
        bool check_actor(const Actor* actor);
        bool erase_actor(std::string name);
        bool erase_actor(Actor* pointer);

        bool collide_terrain(Actor* actor, int& x_max, int& y_max, bool notify = false);
        bool collide_terrain(Actor* actor, bool notify = false);

        bool collide_terrain(const SDL_Rect& rect, int& x_max, int& y_max);
        bool collide_terrain(const SDL_Rect& rect);

        void collision_check();

        std::vector<MapLayer*> get_map_layers();
        std::vector<ImageLayer*> get_image_layers();
        std::vector<ObjectLayer*> get_object_layers();

        Layer* get_layer(std::string name);

        MapData& get_base_map() {return *m_base_map;}

        // Don't allow copy construction and assignment because our destructor would delete twice!
        LayerCollection(const LayerCollection& other) = delete;
        LayerCollection& operator=(const LayerCollection& other) = delete;

        LayerCollection(LayerCollection&& other) = default;
        LayerCollection& operator=(LayerCollection&& other) = default;

    private:
        MapData* m_base_map;
        std::vector<std::unique_ptr<Layer>> m_layers;
};


#endif // LAYER_COLLECTION_HPP_INCLUDED
