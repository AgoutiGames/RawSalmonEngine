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

/*
Actor::Actor(Uint16 tile_id) : Actor::Actor(m_templates.at(m_gid_to_temp_name.at(tile_id)))
{

}
*/

Actor::Actor(const ActorTemplate& templ) :
 m_base_speed {templ.speed},
 m_AI {templ.AI},
 m_direction {templ.direction},
 m_hitbox {templ.hitbox},
 m_animations {templ.animations}
{

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
void Actor::render(int x_cam, int y_cam, const MapData& base_map) const {
    SDL_Rect dest {static_cast<int>(x_cam + m_x), static_cast<int>(y_cam + m_y - m_height), static_cast<int>(m_width), static_cast<int>(m_height)};
    m_animations.at(m_anim_state).at(m_direction).render(dest, base_map);

    // Alternative which doesnt do any resizing
    //m_animations.at(m_anim_state).at(m_direction).render(static_cast<int>(x_cam + m_x), static_cast<int>(y_cam + m_y - m_height), base_map);
}

/**
 * @brief Move actor to a direction by float factors
 * @param x_factor, y_factor Which indicate direction and extent of movement
 * @return a @c bool which indicates collision
 * @todo Check for collision!!!
 */
bool Actor::move(float x_factor, float y_factor) {
    /// @todo Check for collision
    bool success = true;
    constexpr float FPS = 60;
    /// @todo Apply tile speed modifiers
    m_x += x_factor * m_base_speed / FPS;
    m_y += y_factor * m_base_speed / FPS;
    return success;
}

/**
 * @brief Process the event pipeline
 * @return a @c bool which indicates if the actor "died"
 * @todo Implement AI and Player behaviour when event_pipeline is empty
 */
bool Actor::process_events() {
    bool alive = true;
    if(!m_event_pipeline.empty()) {
        ActorEvent* event = m_event_pipeline.front();
        bool processed = event->process(*this);
        if(processed) {
            m_event_pipeline.front()->kill();
            m_event_pipeline.erase(m_event_pipeline.begin());
        }
    }
    else {
        animate(AnimationType::idle, Direction::down);
        // AI and Player behaviour stuff
    }
    return alive;
}

/**
 * @brief Adds the event to the actors pipeline and sorts
 * @param event The event to be added
 */
void Actor::add_event(ActorEvent* event) {
    if(event->priority() == Priority::clear_all) m_event_pipeline.clear();
    if(!m_event_pipeline.empty()) {
        auto it = m_event_pipeline.end();
        do {
            --it;
            if((*it)->priority() >= event->priority()) {
                ++it;
                m_event_pipeline.insert(it, event);
                return;
            }
        } while(it != m_event_pipeline.begin());
    }
    m_event_pipeline.insert(m_event_pipeline.begin(), event);
}

/**
 * @brief Update the actor state
 * @return @c bool which returns true if actor is alive
 * @note Currently only pushes the animation
 */
bool Actor::update() {
    bool alive = process_events();
    return alive;
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c bool which indicates if the animation finished a cycle/wrapped around
 * @warning Currently there is no checking if anim or dir are valid for the actor!!
 *          Segmentation fault possible for not properly parsed actor!!!
 */
bool Actor::animate(AnimationType anim, Direction dir) {
    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        m_animations[m_anim_state][m_direction].init_anim();
    }
    return m_animations[m_anim_state][m_direction].push_anim();
}
