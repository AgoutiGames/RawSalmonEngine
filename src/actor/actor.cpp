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

#include "map/mapdata.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

/*
Actor::Actor(Uint16 tile_id) : Actor::Actor(m_templates.at(m_gid_to_temp_name.at(tile_id)))
{

}
*/

Actor::Actor(const ActorTemplate& templ, const MapData* map) :
 m_map {map},
 m_type {templ.template_name},
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
        eResult = parse_actor_properties(p_property, m_base_speed, m_AI, m_direction);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing actor properties for actor: " << m_name << "\n";
            return eResult;
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
 * @todo Apply tile speed modifiers
 */
bool Actor::move(float x_factor, float y_factor) {
    bool moved = true;
    // Move the Actor
    constexpr float FPS = 60;
    // Determine if it can collide/ has gitbox
    if(!SDL_RectEmpty(&m_hitbox)) {
        // Apply position of actor to hitbox
        SDL_Rect temp = m_hitbox;
        m_x += x_factor * m_base_speed / FPS;
        temp.x += static_cast<int>(m_x);
        if(x_factor != 0) {
            temp.y += static_cast<int>(m_y) - m_height;
            // Check for x-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->collide(&temp, x_inter_depth, y_inter_depth)) {
                // Do stuff with the intersection depth
                if(x_factor < 0) {x_inter_depth = -x_inter_depth;}
                m_x -= x_inter_depth;
                temp.x -= x_inter_depth;
                moved = false;
            }
            temp.y -= static_cast<int>(m_y) - m_height;
        }

        if(y_factor != 0){
            m_y += y_factor * m_base_speed / FPS;
            temp.y += static_cast<int>(m_y) - m_height;
            // Check for y-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->collide(&temp, x_inter_depth, y_inter_depth)) {
                // Do stuff with the intersection depth
                if(y_factor < 0) {y_inter_depth = -y_inter_depth;}
                m_y -= y_inter_depth;
                moved = false;
            }
        }
    }
    return moved;
}

/**
 * @brief Process the event pipeline
 * @return a @c bool which indicates if the actor "died"
 * @todo Implement AI and Player behaviour when event_pipeline is empty
 */
bool Actor::process_events() {
    bool alive = true;
    if(!m_event_pipeline.empty()) {
        for(unsigned i = 0; i < m_event_pipeline.size(); i++) {
            ActorEvent* event = m_event_pipeline[i];
            EventSignal signal = event->process(*this);
            if(signal == EventSignal::stop) {
                break;
            }
            else if(signal == EventSignal::end) {
                m_event_pipeline[i]->kill();
                m_event_pipeline.erase(m_event_pipeline.begin() + i);
                i--;
            }
        }
    }
    else {
        animate(AnimationType::idle, m_direction);
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

/**
 * @brief Returns true if actor collides with rect
 * @note Hitbox width and height should be at least 10px
 *       when max actor speed is 500px per second
 * Returns minimum x and y values to go back to not intersect anymore
 */
bool Actor::collide(const SDL_Rect* rect, int& x_depth, int& y_depth) const{
    if(SDL_RectEmpty(&m_hitbox)) {return false;}
    SDL_Rect temp = m_hitbox;
    temp.x += static_cast<int>(m_x);
    temp.y += static_cast<int>(m_y) - m_height;
    SDL_Rect inter;
    if(SDL_IntersectRect(&temp, rect, &inter) && !SDL_RectEquals(&temp, rect)) {
        x_depth = inter.w;
        y_depth = inter.h;
        /*
        std::cerr << "check " << temp.x << " " << temp.y << " " << temp.w << " " << temp.h << "\n";
        std::cerr << "check " << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << "\n";
        std::cerr << "x depth: " << x_depth << "\n";
        std::cerr << "y_depth: " << y_depth << "\n";
        */
        return true;
    }
    return false;
}
