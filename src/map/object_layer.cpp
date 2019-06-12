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
        eResult = m_obj_grid.back().init_actor(p_object);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading object in layer: " << m_name << " with gid: " << gid << "\n";
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
    int x = camera.x() - m_offset_x;
    int y = camera.y() - m_offset_y;
    TilesetCollection& ts_collection = m_layer_collection->get_base_map().get_ts_collection();
    int from = y - ts_collection.get_overhang(Direction::up) * ts_collection.get_tile_h();
    int to = y + camera.h() + ts_collection.get_overhang(Direction::down) * ts_collection.get_tile_h();
    // Has y-axis offscreen culling
    for(auto it=m_obj_grid.begin(); it != m_obj_grid.end(); ++it) {
        int feet = it->get_y();
        int head = feet - it->get_h();
        if(feet > from && head < to) {it->render(x,y);}
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
    for(auto it=m_obj_grid.begin(); it != m_obj_grid.end(); ++it) {
        if(it->update() == false) {
            it--;
            m_obj_grid.erase(++it);
        }
    }
    // Establish correct rendering order
    m_obj_grid.sort();
}

/**
 * @brief Fetch all actors which conform the supplied parameters
 * @return Vector of conforming actors
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
 * @brief Fetch first actor which conforms the supplied parameters
 * @return Confirming actor
 */
Actor* ObjectLayer::get_actor(std::string name) {
    for(Actor& actor : m_obj_grid) {
        if(actor.get_name() == name) {
            return &actor;
        }
    }
    return nullptr;
}

