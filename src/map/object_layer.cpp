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
#include "map/object_layer.hpp"

#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "map/tileset_collection.hpp"
#include "map/layer_collection.hpp"
#include "map/camera.hpp"

/// Factory function which retrieves a pointer owning the object layer
ObjectLayer* ObjectLayer::parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) {
    return new ObjectLayer(source, name, layer_collection, eresult);
}

/// Constructor which only may be used internally
ObjectLayer::ObjectLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult) : Layer(name, layer_collection) {
    eresult = init(source);
}

/**
 * @brief Initialize the layer by parsing info from source
 * @param source The @c XMLElement from which information is parsed
 * @return @c XMLError which indicates failure or sucess of parsing
 */
tinyxml2::XMLError ObjectLayer::init(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    // Parse individual objects/actors
    XMLElement* p_object = source->FirstChildElement("object");
    while(p_object != nullptr) {
        unsigned gid;
        eResult = p_object->QueryUnsignedAttribute("gid", &gid);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Object in layer: " << m_name << " has no tile id!\n";
            std::cerr << "Currently only tile objects are supported/only true actors\n";
            return eResult;
        }

        // Construct actor from template corresponding to gid
        m_obj_grid.push_back(Actor(gid, &(m_layer_collection->get_base_map() )));

        // Initialize actor from the XMLElement*
        eResult = m_obj_grid.back().parse_base(p_object);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading dimensions and name of object in layer: " << m_name << " with gid: " << gid << "\n";
            return eResult;
        }
        eResult = m_obj_grid.back().parse_properties(p_object);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading properties of object in layer: " << m_name << " with gid: " << gid << "\n";
            return eResult;
        }
        // Move to next object/actor
        p_object = p_object->NextSiblingElement();
    }

    return XML_SUCCESS;
}

/**
 * @brief Renders the object layer to screen according to camera
 * @param camera Our active camera
 * @return @c bool which indicates sucess
 */
bool ObjectLayer::render(const Camera& camera) const {
    for(const Actor* actor : get_clip(camera.get_rect())) {
        actor->render(camera.x(), camera.y());
    }
    return true;
}

/**
 * @brief Updates the states of the objects in the layer
 *
 * This function currently triggers the update function of every
 * object of an object type layer. When it returns false, this is
 * a signal to delete the object.
 */
void ObjectLayer::update() {
    for(Actor& a : m_obj_grid) {
        a.update();
    }
    // Establish correct rendering order
    m_obj_grid.sort();
}

/**
 * @brief return a vector of pointers to each actor
 */
std::vector<Actor*> ObjectLayer::get_actors() {
    std::vector<Actor*> actor_list;
    for(Actor& actor : m_obj_grid) {
        actor_list.push_back(&actor);
    }
    return actor_list;
}

/**
 * @brief Fetch all actors which have the given name
 * @return Vector of pointers to actor
 */
std::vector<Actor*> ObjectLayer::get_actors(std::string name) {
    std::vector<Actor*> actor_list;
    for(Actor& actor : m_obj_grid) {
        if(actor.get_name() == name) {
            actor_list.push_back(&actor);
        }
    }
    return actor_list;
}

/**
 * @brief Fetch first actor which has the name
 * @return Pointer to matching actor
 */
Actor* ObjectLayer::get_actor(std::string name) {
    for(Actor& actor : m_obj_grid) {
        if(actor.get_name() == name) {
            return &actor;
        }
    }
    return nullptr;
}

/**
 * @brief Returns a vector of pointers to actor which contains all actors which are within or intersect with the given rect
 */
std::vector<Actor*> ObjectLayer::get_clip(const SDL_Rect& rect) {

    TilesetCollection& tsc = m_layer_collection->get_base_map().get_ts_collection();
    SDL_Rect window = rect;
    window.w += tsc.get_tile_w() + tsc.get_overhang(Direction::left) + tsc.get_overhang(Direction::right);
    window.h += tsc.get_tile_h() + tsc.get_overhang(Direction::up) + tsc.get_overhang(Direction::down);
    window.x -= tsc.get_tile_w() + tsc.get_overhang(Direction::left);
    // Because origin of actor isn't upper left, but lower left, we don't need this
    //window.y -= tsc.get_tile_h() + tsc.get_overhang(Direction::up) * tsc.get_tile_h();

    std::vector<Actor*> actor_list;
    for(Actor& actor : m_obj_grid) {
        // This could possibly refactored for performance gain
        if(actor.get_x() > window.x &&
           actor.get_x() < window.x + window.w &&
           actor.get_y() > window.y &&
           actor.get_y() < window.y + window.h)
           {
            actor_list.push_back(&actor);
           }
    }
    return actor_list;
}

/// Const variant of get_clip(), needed for constant render() function
std::vector<const Actor*> ObjectLayer::get_clip(const SDL_Rect& rect) const {
    TilesetCollection& tsc = m_layer_collection->get_base_map().get_ts_collection();
    SDL_Rect window = rect;
    window.w += tsc.get_tile_w() + tsc.get_overhang(Direction::left) + tsc.get_overhang(Direction::right);
    window.h += tsc.get_tile_h() + tsc.get_overhang(Direction::up) + tsc.get_overhang(Direction::down);
    window.x -= tsc.get_tile_w() + tsc.get_overhang(Direction::left);
    // Because origin of actor isn't upper left, but lower left, we don't need this
    //window.y -= tsc.get_tile_h() + tsc.get_overhang(Direction::up);

    std::vector<const Actor*> actor_list;
    for(const Actor& actor : m_obj_grid) {
        if(actor.get_x() > window.x &&
           actor.get_x() < window.x + window.w &&
           actor.get_y() > window.y &&
           actor.get_y() < window.y + window.h) {
            actor_list.push_back(&actor);
        }
    }
    return actor_list;
}

/// Remove actor with given name from layer
bool ObjectLayer::erase_actor(std::string name) {
    for (auto itr = m_obj_grid.begin(); itr != m_obj_grid.end(); itr++) {
        if ((*itr).get_name() == name) {
            itr = m_obj_grid.erase(itr);
            return true;
        }
    }
    return false;
/*
    for(Actor& a : m_obj_grid) {
        if(a.get_name() == name) {
            m_obj_grid.remove(a);
            return true;
        }
    }
    return false;
    */
}

/// Remove actor with given pointer from layer
bool ObjectLayer::erase_actor(Actor* actor) {
    for (auto itr = m_obj_grid.begin(); itr != m_obj_grid.end(); itr++) {
        if (&(*itr) == actor) {
            itr = m_obj_grid.erase(itr);
            return true;
        }
    }
    return false;

    /*
    for(Actor& a : m_obj_grid) {
        if(&a == actor) {
            m_obj_grid.remove(a);
            return true;
        }
    }
    return false;*/
}
