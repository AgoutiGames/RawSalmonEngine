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
#include "actor/actor.hpp"

#include <string>
#include <iostream>
#include <map>

#include "event/ae_move_direction.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

std::map<std::string, ActorTemplate> Actor::m_templates;
std::map<Uint16, std::string> Actor::m_gid_to_temp_name;

Actor::Actor(Uint16 tile_id) : Actor::Actor(m_templates.at(m_gid_to_temp_name.at(tile_id)))
{

}

Actor::Actor(ActorTemplate& templ) :
 m_base_speed {templ.speed},
 m_AI {templ.AI},
 m_direction {templ.direction},
 m_hitbox {templ.hitbox},
 m_animations {templ.animations}
{

}

/**
 * @brief Initialize the whole actor class
 */
void Actor::initialize() {
    m_templates.clear();
    m_gid_to_temp_name.clear();
    // Initialize all actor events
    AeMoveDirection::initialize_all();
}

/**
 * @brief Initialize actor information from XML info
 * @param source The @c XMLElement which contains the information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Actor::init_actor(tinyxml2::XMLElement* source) {

    using namespace tinyxml2;
    XMLError eResult;

    // Parse position and size of actor
    eResult = source->QueryFloatAttribute("x", &m_x);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryFloatAttribute("y", &m_y);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;

    // Parse the (unique) name of the actor
    const char* p_actor_name;
    p_actor_name = source->Attribute("name");
    if(p_actor_name == nullptr) {
        std::cerr << "Actor at x: " << m_x << " y: " << m_y << " is missing a custom name!\n";
        return XML_NO_ATTRIBUTE;
    }
    m_name = p_actor_name;

    // Parse user specified properties of the actor
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");

        p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

            // Parse base speed
            else if(name == "BASE_SPEED") {
                eResult = p_property->QueryFloatAttribute("value", &m_base_speed);
                if(eResult != XML_SUCCESS) {
                    std::cerr << "Failed at loading speed value for actor : " << m_name << "\n";
                    return eResult;
                }
            }

            //Parse current direction facing
            else if(name == "DIRECTION") {
                const char* p_direction = p_property->Attribute("value");
                if(p_direction != nullptr) {
                    m_direction = str_to_direction(std::string(p_direction));
                    if(m_direction == Direction::invalid) {
                        std::cerr << "Invalid direction type \"" << p_direction << "\" in actor " << m_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    std::cerr << "Empty direction for actor " << m_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }

            }

            // Parse the AI type
            else if(name == "BEHAVIOUR") {
                const char* p_behaviour = p_property->Attribute("value");
                if(p_behaviour != nullptr) {
                    m_AI = str_to_behaviour(std::string(p_behaviour));
                    if(m_AI == Behaviour::invalid) {
                        std::cerr << "Invalid behaviour type \"" << p_behaviour << "\" in actor " << m_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    std::cerr << "Empty behaviour for actor " << m_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }
            }

            else {
                std::cerr << "Unknown actor property \""<< p_name << "\" specified\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Render the actor at it's position relative to the camera position
 */
void Actor::render(int x_cam, int y_cam) const {
    SDL_Rect dest {static_cast<int>(x_cam + m_x), static_cast<int>(y_cam + m_y - m_height), static_cast<int>(m_width), static_cast<int>(m_height)};
    m_animations.at(m_anim_state).at(m_direction).render(dest);
}

/**
 * @brief Adds a copy of an animation tile to an actor template
 * @param name Name of the @c ActorTemplate
 * @param anim The @c AnimationType of the tile
 * @param dir The @c Direction of the tile
 * @param tile A pointer to the corresponding animation tile
 */
void Actor::add_animation(std::string name, AnimationType anim, Direction dir, Tile* tile) {
    // Get a reference to the ActorTemplate from its name
    ActorTemplate& templ = m_templates[name];

    // Add a copy of the tile to the ActorTemplate
    templ.animations[anim][dir] = *tile;

    // Initialize the animation state of the copied tile
    templ.animations[anim][dir].init_anim();
}

/**
 * @brief Add an @c ActorTemplate to the static vector @c m_templates from an @c XMLElement
 * @param source The @c XMLElement which contains the information
 * @param tile_id The corresponding tile_id to the @c ActorTemplate
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Actor::add_template(tinyxml2::XMLElement* source, Uint16 tile_id) {
    using namespace tinyxml2;
    XMLError eResult;

    std::string actor_name = "_";


    // Parse user specified properties of the ActorTemplate
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");

        // First check for name because this is a prequisite for parsing the remaining information
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

            // Parse name of the ActorTemplate
            else if(name == "ACTOR_NAME") {
                const char* p_actor_name = p_property->Attribute("value");
                if(p_actor_name != nullptr) {

                    // Register the ActorTemplate
                    actor_name = p_actor_name;
                    m_templates[actor_name].template_name = actor_name;
                    m_gid_to_temp_name[tile_id] = actor_name;
                }
                else {
                    std::cerr << "Invalid actor name in actor template\n";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }
            p_property = p_property->NextSiblingElement("property");
        }

        if(actor_name == "_") {
            std::cerr << "Missing actor name in actor template \n";
            return XML_NO_ATTRIBUTE;
        }

        // Parse user specified properties of the ActorTemplate
        p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

            //Skip because name was already parsed
            else if(name == "ACTOR_NAME") {}

            // Parse default base speed
            else if(name == "BASE_SPEED") {
                float speed;
                eResult = p_property->QueryFloatAttribute("value", &speed);
                if(eResult != XML_SUCCESS) {
                    std::cerr << "Failed at loading speed value for actor template: " << actor_name << "\n";
                    return eResult;
                }
                m_templates[actor_name].speed = speed;
            }

            //Parse default direction facing
            else if(name == "DIRECTION") {
                const char* p_direction = p_property->Attribute("value");
                if(p_direction != nullptr) {
                    Direction dir;
                    dir = str_to_direction(std::string(p_direction));
                    if(dir == Direction::invalid) {
                        std::cerr << "Invalid direction type \"" << p_direction << "\" in actor template for " << actor_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                    m_templates[actor_name].direction = dir;
                }
                else {
                    std::cerr << "Empty direction in actor template for " << actor_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }

            }

            // Parse default AI type
            else if(name == "BEHAVIOUR") {
                const char* p_behaviour = p_property->Attribute("value");
                if(p_behaviour != nullptr) {
                    Behaviour beh;
                    beh = str_to_behaviour(std::string(p_behaviour));
                    if(beh == Behaviour::invalid) {
                        std::cerr << "Invalid behaviour type \"" << p_behaviour << "\" in actor template for " << actor_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                    m_templates[actor_name].AI = beh;
                }
                else {
                    std::cerr << "Empty behaviour in actor template for " << actor_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }
            }

            else {
                std::cerr << "Unknown actor template property \""<< p_name << "\" specified\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }

    else {
        std::cerr << "Missing properties on actor_animation tile\n";
        return XML_NO_ATTRIBUTE;
    }

    // Parse the default hitbox
    XMLElement* p_objgroup = source->FirstChildElement("objectgroup");
    if(p_objgroup != nullptr) {
        XMLElement* p_object = p_objgroup->FirstChildElement("object");
        if(p_object != nullptr) {
            SDL_Rect temp_rec;
            float temp;
            eResult = p_object->QueryFloatAttribute("x", &temp);
            if(eResult != XML_SUCCESS) return eResult;
            temp_rec.x = static_cast<int>(temp);
            eResult = p_object->QueryFloatAttribute("y", &temp);
            if(eResult != XML_SUCCESS) return eResult;
            temp_rec.y = static_cast<int>(temp);
            eResult = p_object->QueryFloatAttribute("width", &temp);
            if(eResult != XML_SUCCESS) return eResult;
            temp_rec.w = static_cast<int>(temp);
            eResult = p_object->QueryFloatAttribute("height", &temp);
            if(eResult != XML_SUCCESS) return eResult;
            temp_rec.h = static_cast<int>(temp);
            m_templates[actor_name].hitbox = temp_rec;
        }
    }

    return XML_SUCCESS;
}

/**
 * @brief Add an @c ActorTemplate to the static vector @c m_templates from an @c XMLElement
 * @param x_factor, y_factor Which indicate direction and extent of movement
 * @return a @c bool which indicates collision
 * @todo Check for collision!!!
 */
bool Actor::move(float x_factor, float y_factor) {
    /// @todo Check for collision
    bool success = true;
    m_x += x_factor * m_base_speed;
    m_y += y_factor * m_base_speed;
    return success;
}

/**
 * @brief Process the event pipeline
 * @return a @c bool which indicates if the actor "died"
 */
bool Actor::process_events() {
    bool alive = true;
    while(!m_event_pipeline.empty()) {
        bool processed = m_event_pipeline.front()->process(*this);
        if(processed) {
    //        m_event_pipeline.front()->kill();
            m_event_pipeline.erase(m_event_pipeline.begin());
        }
    }
    return alive;
}

/**
 * @brief Adds the event to the actors pipeline and sorts
 * @param event The event to be added
 */
void Actor::add_event(ActorEvent* event) {
    m_event_pipeline.push_back(event);
}

/**
 * @brief Sorts the event pipeline by priority values
 */
void Actor::sort_events() {

}


/**
 * @brief Update the actor state
 * @return @c bool which returns true if actor is alive
 * @note Currently only pushes the animation
 */
bool Actor::update() {
    bool alive = process_events();
    m_animations[m_anim_state][m_direction].push_anim();
    return alive;
}
