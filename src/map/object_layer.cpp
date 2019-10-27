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
#include "actor/primitive.hpp"
#include "map/mapdata.hpp"
#include "map/tileset_collection.hpp"
#include "map/layer_collection.hpp"
#include "map/camera.hpp"
#include "util/logger.hpp"

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
    MapData& mapdata = m_layer_collection->get_base_map();

    // Parse user specified properties of the object_layer (only suspended right now)
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            else if(name == "SUSPENDED") {
                eResult = p_property->QueryBoolAttribute("value", &m_suspended);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Failed parsing SUSPENDED attribute";
                    return eResult;
                }
            }

            else {
                Logger(Logger::error) << "Unknown tile property \""<< p_name << "\" specified";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            p_property = p_property->NextSiblingElement("property");
        }
    }

    // Parse individual objects/actors
    XMLElement* p_object = source->FirstChildElement("object");
    while(p_object != nullptr) {
        unsigned gid;
        eResult = p_object->QueryUnsignedAttribute("gid", &gid);

        if(eResult == XML_SUCCESS && mapdata.is_actor(gid)) {

            add_actor(Actor(mapdata.get_actor(gid)));

            // m_obj_grid.push_back(Actor(mapdata.get_actor(gid)));

            // Initialize actor from the XMLElement*
            eResult = m_obj_grid.back().parse_base(p_object);
            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Failed at loading dimensions and name of object in layer: " << m_name << " with gid: " << gid;
                return eResult;
            }
            eResult = m_obj_grid.back().parse_properties(p_object);
            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Failed at loading properties of object in layer: " << m_name << " with gid: " << gid;
                return eResult;
            }
        }
        else {

            Primitive* p = Primitive::parse(p_object, mapdata);
            if(p == nullptr) {
                Logger(Logger::warning) << "Couldn't load primitive object with id " << p_object->Attribute("id") << ", skipping";
            }
            else {
                add_primitive(p);
            }
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
    if(m_hidden) {return true;}
    for(const Actor* actor : get_clip(camera.get_rect())) {
        actor->render(camera.x(), camera.y());
    }

    // For now just always render all primitives
    // Offscreen culling has to be implemented yet
    for(auto& primitive : m_primitives) {
        primitive->render(camera.x(), camera.y());
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
void ObjectLayer::update(bool late) {
    if(m_suspended) {return;}
    for(Actor& a : m_obj_grid) {
        if(a.late_polling() == late) {
            a.update();
        }
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

void ObjectLayer::add_actor(Actor a) {
    a.respond(Response::on_activation);
    m_obj_grid.push_back(a);
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

void ObjectLayer::add_primitive(Primitive* primitive) {
    m_primitives.emplace_back(primitive);
}

Primitive* ObjectLayer::get_primitive(std::string name) const {
    for(auto& elem : m_primitives) {
        if(elem->get_name() == name) {
            return elem.get();
        }
    }
    return nullptr;
}

std::vector<Primitive*> ObjectLayer::get_primitives() {
    std::vector<Primitive*> primitives;
    for(auto& p : m_primitives) {
        primitives.push_back(p.get());
    }
    return primitives;
}

bool ObjectLayer::erase_primitive(std::string name) {
    for(auto elem = m_primitives.begin(); elem != m_primitives.end(); elem++) {
        if((*elem)->get_name() == name) {
            m_primitives.erase(elem);
            return true;
        }
    }
    return false;
}
