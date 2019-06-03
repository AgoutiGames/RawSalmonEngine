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

#include "map/layer.hpp"
#include "map/map_layer.hpp"
#include "map/image_layer.hpp"
#include "map/object_layer.hpp"
#include "map/layer_collection.hpp"

/**
 * @brief Deletes all layers because we own each layer by its pointer
 */
LayerCollection::~LayerCollection() {
    purge();
}

/**
 * @brief Deletes all layers because we own each layer by its pointer
 */
void LayerCollection::purge() {
    for(Layer* layer : m_layers) {
        if(layer != nullptr) {
            delete layer;
        }
    }
}

LayerCollection::LayerCollection(LayerCollection&& other) : m_layers{other.m_layers} {
    for(int i = 0; i < other.m_layers.size(); i++) {
        other.m_layers[i] = nullptr;
    }
}

LayerCollection& LayerCollection::operator=(LayerCollection&& other) {
    if(this != &other) {
        purge();
        m_base_map = other.m_base_map;
        m_layers = other.m_layers;
        for(int i = 0; i < other.m_layers.size(); i++) {
            other.m_layers[i] = nullptr;
        }
    }
    return *this;
}

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
    m_layers.resize(p_layers.size(), nullptr);

    // Actually parse each layer of the vector of pointers
    for(unsigned i_layer = 0; i_layer < p_layers.size(); i_layer++) {
        XMLError eResult = XML_SUCCESS;
        m_layers[i_layer] = Layer::parse(p_layers[i_layer], this, eResult);
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
 * @brief Updates each layer state
 */
void LayerCollection::update() {
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        m_layers[i_layer]->update();
    }
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param x_max, y_max The maximum depth of intersection by axis
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool LayerCollection::collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    int x_depth = 0;
    int y_depth = 0;
    // Iterate through all layers
    for(Layer* layer : m_layers) {
        if(layer->collide(rect, x_depth, y_depth, collided, type)) {
            if(x_depth > x_max) {x_max = x_depth;}
            if(y_depth > y_max) {y_max = y_depth;}
            collide = true;
        }
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool LayerCollection::collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    // Iterate through all layers
    for(Layer* layer : m_layers) {
        if(layer->collide(rect, collided, type)) {
            collide = true;
        }
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool LayerCollection::collide(const SDL_Rect* rect, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    // Iterate through all layers
    for(Layer* layer : m_layers) {
        if(layer->collide(rect, type)) {
            collide = true;
        }
    }
    return collide;
}

/**
 * @brief Fetches all actors which conform the supplied parameters
 * @return Vector of conforming actors
 * @note "invalid" value indicates that a parameter is ignored
 */
std::vector<Actor*> LayerCollection::get_actors(std::string name, Direction direction, AnimationType animation) {
    std::vector<Actor*> actor_list;
    for(Layer* layer : m_layers) {
        if(layer->get_type() == Layer::object) {
            ObjectLayer* ob_layer = static_cast<ObjectLayer*>(layer);
            std::vector<Actor*> sublist = ob_layer->get_actors(name, direction, animation);
            actor_list.insert(actor_list.end(),sublist.begin(),sublist.end());
        }
    }
    return actor_list;
}
