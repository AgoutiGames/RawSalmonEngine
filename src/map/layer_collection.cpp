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
#include <iostream>

#include "actor/actor.hpp"
#include "map/layer.hpp"
#include "map/mapdata.hpp"
#include "map/map_layer.hpp"
#include "map/image_layer.hpp"
#include "map/object_layer.hpp"
#include "map/layer_collection.hpp"
#include "map/tile.hpp"

/**
 * @brief Parses each layer and stores in vector member
 * @param source The @c XMLElement which stores the layer info
 * @param base_map The map which we belong to
 * @return an @c XMLError object which indicates success or error type
 */
 tinyxml2::XMLError LayerCollection::init(tinyxml2::XMLElement* source, MapData& base_map) {

    using namespace tinyxml2;
    m_base_map = &base_map;

    // Collect all layers to a vector of pointers
    std::vector<XMLElement*> p_layers;
    if (source == nullptr) {
        std::cerr << "Mapfile has no layers\n";
        return XML_ERROR_PARSING_ELEMENT;
    }

    do{
        p_layers.push_back(source);
        source = source->NextSiblingElement();
    }while(source != nullptr );
    std::cout << "Layer count: " << p_layers.size() << "\n";


    /// Don't forget to implement the new pointer inheritance approach
    // Clear layer vector member of possible old data
    m_layers.clear();
    m_layers.reserve(p_layers.size());

    // Actually parse each layer of the vector of pointers
    for(unsigned i_layer = 0; i_layer < p_layers.size(); i_layer++) {
        XMLError eResult = XML_SUCCESS;
        m_layers.emplace_back(Layer::parse(p_layers[i_layer], this, eResult));
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing layer: " << i_layer << "\n";
            return eResult;
        }
    }
    return XML_SUCCESS;
 }

 /**
  * @brief Renders each layer to screen with current camera position and size
  * @param camera Our active camera
  * @return @c bool Which indicates sucess or failure of rendering
  */
bool LayerCollection::render(const Camera& camera) const{
    bool success = true;
    // Renders all layers
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        if(!m_layers[i_layer]->render(camera)) {
            std::cerr << "Failed at rendering layer " << i_layer << " !\n";
            success = false;
        }
    }
    return success;
}

/**
 * @brief Updates each object layer state
 */
void LayerCollection::update() {
    for(auto layer : get_object_layers()) {
        layer->update();
    }
}

/**
 * @brief checks if a given rect collides with the default hitboxes of the map layer
 */
bool LayerCollection::collide_terrain(const SDL_Rect& rect, int& x_max, int& y_max) {
    if(SDL_RectEmpty(&rect)) {return false;}
    bool collide = false;
    // Iterate through all map_layers
    for(auto layer : get_map_layers()) {
        for(auto t : layer->clip(rect)) {
            if(std::get<0>(t) != 0) {
                Tile* tile = m_base_map->get_ts_collection().get_tile(std::get<0>(t));
                SDL_Rect tile_rect = tile->get_hitbox("COLLIDE",true);
                // Only check collision for tiles with valid hitbox
                if(!SDL_RectEmpty(&tile_rect)) {
                    // Move tile hitbox to tile coordinates
                    tile_rect.x += std::get<1>(t);
                    tile_rect.y += std::get<2>(t);

                    SDL_Rect relative {0,0,rect.w,rect.h};
                    SDL_Rect intersect;
                    // Get intersection from supplied rect and tile rect
                    if(SDL_IntersectRect(&relative, &tile_rect, &intersect)) {
                        // Possibly overwrite maximum collision depth value
                        if(intersect.w > x_max) {x_max = intersect.w;}
                        if(intersect.h > y_max) {y_max = intersect.h;}

                        collide = true;
                    }
                }
            }
        }
    }
    return collide;
}

/**
 * @brief checks if a given rect collides with the default hitboxes of the map layer
 */
bool LayerCollection::collide_terrain(const SDL_Rect& rect) {
    int x,y;
    return collide_terrain(rect, x,y);
}

/**
 * @brief checks if a given actor collides with the default hitboxes of the map layer
 */
bool LayerCollection::collide_terrain(Actor* actor, int& x_max, int& y_max, bool notify) {
    SDL_Rect rect = actor->get_hitbox("COLLIDE");
    if(SDL_RectEmpty(&rect)) {return false;}
    bool collide = false;
    // Iterate through all map_layers
    for(auto layer : get_map_layers()) {
        for(auto t : layer->clip(rect)) {
            if(std::get<0>(t) != 0) {
                Tile* tile = m_base_map->get_ts_collection().get_tile(std::get<0>(t));
                SDL_Rect tile_rect = tile->get_hitbox("COLLIDE", true);
                // Only check collision for tiles with valid hitbox
                if(!SDL_RectEmpty(&tile_rect)) {
                    // Move tile hitbox to tile coordinates
                    tile_rect.x += std::get<1>(t);
                    tile_rect.y += std::get<2>(t);

                    SDL_Rect relative {0,0,rect.w,rect.h};
                    SDL_Rect intersect;
                    // Get intersection from supplied rect and tile rect
                    if(SDL_IntersectRect(&relative, &tile_rect, &intersect)) {
                        // Possibly overwrite maximum collision depth value
                        if(intersect.w > x_max) {x_max = intersect.w;}
                        if(intersect.h > y_max) {y_max = intersect.h;}

                        collide = true;
                        if(notify) {
                            Cause c = Cause(tile, "COLLIDE", "COLLIDE");
                            actor->respond(Response::on_collision, c);
                        }
                    }
                }
            }
        }
    }
    return collide;
}

/**
 * @brief checks if a given actor collides with the default hitboxes of the map layer
 */
bool LayerCollection::collide_terrain(Actor* actor, bool notify) {
    int x,y;
    return collide_terrain(actor, x,y, notify);
}

/**
 * @brief Fetches all actors
 * @return Vector of all actors
 */
std::vector<Actor*> LayerCollection::get_actors() {
    std::vector<Actor*> actor_list;
    for(auto& layer : get_object_layers()) {
        std::vector<Actor*> sublist = layer->get_actors();
        actor_list.insert(actor_list.end(),sublist.begin(),sublist.end());
    }
    return actor_list;
}

/**
 * @brief Fetches all actors which conform the supplied parameters
 * @return Vector of conforming actors
 */
std::vector<Actor*> LayerCollection::get_actors(std::string name) {
    std::vector<Actor*> actor_list;
    for(auto& layer : get_object_layers()) {
        std::vector<Actor*> sublist = layer->get_actors(name);
        actor_list.insert(actor_list.end(),sublist.begin(),sublist.end());
    }
    return actor_list;
}

/**
 * @brief Fetches first actor which conforms the supplied parameter
 * @return Confirming Actor
 */
Actor* LayerCollection::get_actor(std::string name) {
    for(auto& layer : get_object_layers()) {
        Actor* a = layer->get_actor(name);
        if(a != nullptr) {return a;}
    }
    return nullptr;
}

/**
 * @brief Fetches all MapLayers
 * @return Vector of MapLayer pointers
 */
std::vector<MapLayer*> LayerCollection::get_map_layers() {
    std::vector<MapLayer*> layer_list;
    for(auto& layer : m_layers) {
        if(layer->get_type() == Layer::map) {
            layer_list.push_back(static_cast<MapLayer*>(layer.get()));
        }
    }
    return layer_list;
}

/**
 * @brief Fetches all ImageLayer
 * @return Vector of ImageLayer pointers
 */
std::vector<ImageLayer*> LayerCollection::get_image_layers() {
    std::vector<ImageLayer*> layer_list;
    for(auto& layer : m_layers) {
        if(layer->get_type() == Layer::image) {
            layer_list.push_back(static_cast<ImageLayer*>(layer.get()));
        }
    }
    return layer_list;
}

/**
 * @brief Fetches all ObjectLayers
 * @return Vector of ObjectLayer pointers
 */
std::vector<ObjectLayer*> LayerCollection::get_object_layers() {
    std::vector<ObjectLayer*> layer_list;
    for(auto& layer : m_layers) {
        if(layer->get_type() == Layer::object) {
            layer_list.push_back(static_cast<ObjectLayer*>(layer.get()));
        }
    }
    return layer_list;
}
