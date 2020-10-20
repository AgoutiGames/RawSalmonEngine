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
#include <iostream>

#include "actor/actor.hpp"
#include "map/layer.hpp"
#include "map/mapdata.hpp"
#include "map/map_layer.hpp"
#include "map/image_layer.hpp"
#include "map/object_layer.hpp"
#include "map/layer_collection.hpp"
#include "map/tile.hpp"
#include "core/gameinfo.hpp"
#include "util/logger.hpp"

namespace salmon { namespace internal {

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
        // std::cerr << "Mapfile has no layers\n";
        /// @note Empty layer_collection is okay now
        return XML_SUCCESS;
    }

    do{
        p_layers.push_back(source);
        source = source->NextSiblingElement();
    }while(source != nullptr );

    /// Don't forget to implement the new pointer inheritance approach
    // Clear layer vector member of possible old data
    m_layers.clear();
    m_layers.reserve(p_layers.size());

    // Actually parse each layer of the vector of pointers
    for(unsigned i_layer = 0; i_layer < p_layers.size(); i_layer++) {
        XMLError eResult = XML_SUCCESS;
        m_layers.emplace_back(Layer::parse(p_layers[i_layer], this, eResult));
        if(eResult != XML_SUCCESS) {
            Logger(Logger::error) << "Failed at parsing layer: " << i_layer;
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
            Logger(Logger::error) << "Failed at rendering layer " << i_layer << " !";
            success = false;
        }
    }
    return success;
}

/**
 * @brief Updates each object layer state
 *
 * First poll possible actor - actor, actor - tile and actor - mouse intersections
 * Then call update for each object layer (Establishes correct render order for actors)
 * @note Doesn't poll collisions on late updates
 */
void LayerCollection::update(bool late) {
    if(!late) {
        // Add possible collisions to actors
        collision_check();
        mouse_collision();
    }
    for(auto layer : get_object_layers()) {
        layer->update(late);
    }


}

/**
 * @brief return a vector of pointers to each actor of each object layer
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
 * @brief Fetch all actors which have the given name
 * @return Vector of pointers to actor
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
 * @brief Fetch first actor which has the name
 * @return Pointer to matching actor
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

/**
 * @brief Adds collisions for actor -- actor and actor -- tile hitbox intersections
 */
void LayerCollection::collision_check() {
    // Iterate over the hitboxes of all actors
    std::vector<Actor*> actors = get_actors();
    if(actors.empty()) {return;}
    for(unsigned i = 0; i < actors.size()-1; i++) {
        for(unsigned j = i + 1; j < actors.size(); j++) {
            actors[i]->check_collision(*actors[j],true);
        }
    }

    for(Actor* actor : actors) {
        SDL_Rect bounds = actor->get_transform().to_bounding_box();
        for(MapLayer* layer : get_map_layers()) {
            for(auto tile : layer->get_clip(bounds)) {
                actor->check_collision(tile,true);
            }
        }
    }
}

/// Returns true if the given actor exists
bool LayerCollection::check_actor(const Actor* actor) {
    for(Actor* a : get_actors()) {
        if(a == actor) {return true;}
    }
    return false;
}

/// Erase the actor from object layer
bool LayerCollection::erase_actor(Actor* pointer) {
    for(ObjectLayer* l : get_object_layers()) {
        if(l->erase_actor(pointer)) {
            return true;
        }
    }
    return false;
}

/// Erase the actor from object layer
bool LayerCollection::erase_actor(std::string name) {
    for(ObjectLayer* l : get_object_layers()) {
        if(l->erase_actor(name)) {
            return true;
        }
    }
    return false;
}

/// Return layer with the given name
Layer* LayerCollection::get_layer(std::string name) {
    for(auto& l : m_layers) {
        if(l->get_name() == name) {
            return l.get();
        }
    }
    return nullptr;
}

/**
 * @brief Tests if any actors hitbox intersects with the mouse pointer location. If yes, adds a collision to the actor.
 */
void LayerCollection::mouse_collision() {
    SDL_Rect cam = m_base_map->get_camera().get_rect();
    // Transform cursor from camera space to global space
    MouseState mouse = m_base_map->get_game().get_input_cache().get_mouse_state();
    SDL_Point click{mouse.x_pos + cam.x, mouse.y_pos+cam.y};

    // Fetch all currently visible actors
    std::vector<Actor*> actors;
    auto layers = get_object_layers();
    for(ObjectLayer* layer : layers) {
        std::vector<Actor*> temp = layer->get_clip(cam);
        actors.insert(actors.end(),temp.begin(),temp.end());
    }

    // Check all hitboxes of each actor if they intersect with the mouse cursor
    for(Actor* a : actors) {
        for(std::pair<std::string, SDL_Rect> hitbox : a->get_hitboxes()) {
            if(SDL_PointInRect(&click,&hitbox.second)) {
                // Trigger the OnMouse response
                // a->respond(Response::on_mouse, Collision(hitbox.first));
                a->add_collision(Collision(hitbox.first));
            }
        }
    }
}

/**
 * @brief Tests if hitbox collides with specified targets hitboxes
 * @param rect The box to check collision for
 * @param target Enum value which tells if to check against tiles, actors or both
 * @param other_hitboxes A vector of hitbox names to check against collision
 * @return true if there is any collision and false if there is none
 */
bool LayerCollection::check_collision(SDL_Rect rect, Collidees target, const std::vector<std::string>& other_hitboxes) {
    bool collided = false;
    if(target == Collidees::tile || target == Collidees::tile_and_actor) {
        for(MapLayer* map : get_map_layers()) {
            for(TileInstance& tile : map->get_clip(rect)) {
                for(std::string hitbox_name : other_hitboxes) {
                    SDL_Rect other_rect = tile.get_hitbox(hitbox_name);
                    if(SDL_HasIntersection(&rect,&other_rect)) {collided = true;}
                }
            }
        }
    }
    if(target == Collidees::actor || target == Collidees::tile_and_actor) {
        for(ObjectLayer* obj : get_object_layers()) {
            for(Actor* actor : obj->get_clip(rect)) {
                for(std::string hitbox_name : other_hitboxes) {
                    SDL_Rect other_rect = actor->get_hitbox(hitbox_name);
                    if(SDL_HasIntersection(&rect,&other_rect)) {collided = true;}
                }
            }
        }
    }
    return collided;
}

}} // namespace salmon::internal
