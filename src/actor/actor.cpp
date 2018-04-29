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


Actor::Actor(Uint16 tile_id, MapData* map) : Actor::Actor(map->get_actor_template(tile_id), map)
{

}


Actor::Actor(const ActorTemplate& templ, MapData* map) :
 m_map {map},
 m_type {templ.template_name},
 m_base_speed {templ.speed},
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
        eResult = m_map->parse_actor_properties(p_property, m_base_speed, m_direction, m_response);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing actor properties for actor: " << m_name << "\n";
            return eResult;
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Render the actor at it's position relative to the camera position
 * @param x_cam, y_cam The coordinates of the upper left corner of the camera rect
 */
void Actor::render(int x_cam, int y_cam) const {
    SDL_Rect dest {static_cast<int>(m_x - x_cam), static_cast<int>(m_y - m_height - y_cam), static_cast<int>(m_width), static_cast<int>(m_height)};
    m_animations.at(m_anim_state).at(m_direction).render(dest, *m_map);

    // Alternative which doesnt do any resizing
    //m_animations.at(m_anim_state).at(m_direction).render(static_cast<int>(x_cam + m_x), static_cast<int>(y_cam + m_y - m_height), base_map);
}

/**
 * @brief Move actor to a direction by float factors
 * @param x_factor, y_factor Which indicate direction and extent of movement
 * @return a @c bool which indicates collision
 * @todo Apply tile speed modifiers
 */
bool Actor::move(float x_factor, float y_factor, bool absolute) {
    bool moved = true;
    // Move the Actor
    constexpr float FPS = 60;
    SDL_Rect temp = get_hitbox();
    float x_step;
    float y_step;
    if(absolute) {
        x_step = x_factor;
        y_step = y_factor;
    }
    else {
        x_step = x_factor * m_base_speed / FPS;
        y_step = y_factor * m_base_speed / FPS;
    }

    // Determine if it can collide/ has hitbox
    if(!SDL_RectEmpty(&temp)) {

        std::vector<Actor*> collided;

        // Apply position of actor to hitbox
        m_x += x_step;
        // Apply x movement
        temp.x += static_cast<int>(m_x);
        // Check for x_axis collision
        if(x_factor != 0) {
            // Apply y movement
            temp.y += static_cast<int>(m_y) - m_height;
            // Check for x-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->collide(&temp, x_inter_depth, y_inter_depth, collided)) {
                // Do stuff with the intersection depth
                if(x_factor < 0) {x_inter_depth = -x_inter_depth;}
                m_x -= x_inter_depth;
                temp.x -= x_inter_depth;
                moved = false;
            }
            // Undo y movement
            temp.y = get_hitbox().y;
        }
        // Check for y_axis collision
        if(y_factor != 0){
            // Apply y movement
            m_y += y_step;
            temp.y += static_cast<int>(m_y) - m_height;
            // Check for y-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->collide(&temp, x_inter_depth, y_inter_depth, collided)) {
                // Do stuff with the intersection depth
                if(y_factor < 0) {y_inter_depth = -y_inter_depth;}
                m_y -= y_inter_depth;
                moved = false;
            }
        }
        // Trigger collision events for each colliding actor, including this one
        for(Actor* a : collided) {
            a->respond(Response::on_collision, this);
            respond(Response::on_collision, a);
        }
    }
    else {
        m_x += x_step;
        m_y += y_step;
    }
    return moved;
}

/**
 * @brief Process the event pipeline
 * @return a @c bool which indicates if the actor should be erased
 */
bool Actor::process_events() {
    if(!m_event_pipeline.empty()) {
        for(unsigned i = 0; i < m_event_pipeline.size(); i++) {
            ActorEvent* event = m_event_pipeline[i];
            EventSignal signal = event->process(*this);
            if(signal == EventSignal::stop) {
                break;
            }
            else if(signal == EventSignal::end || signal == EventSignal::abort) {
                m_event_pipeline[i]->kill();
                m_event_pipeline.erase(m_event_pipeline.begin() + i);
                i--;
            }
            else if(signal == EventSignal::erase) {return false;}
        }
    }
    if(m_event_pipeline.empty()) {
        respond(Response::on_idle);
    }
    return true;
}

/**
 * @brief Adds the event to the actors pipeline and sorts
 * @param event The event to be added
 */
void Actor::add_event(ActorEvent* event) {
    if(!is_blocked(event->get_type()) && !is_blocked(event->name())
       && !is_blocked(event->get_key())
       && !in_cooldown(event->get_type()) && !in_cooldown(event->name())) {
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
        return;
    }
}

/**
 * @brief Update the actor state
 * @return @c bool which returns true if actor is alive
 */
bool Actor::update() {
    respond(Response::on_always);
    bool alive = process_events();
    return alive;
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c bool which indicates if the animation finished a cycle/wrapped around
 */
bool Actor::animate(AnimationType anim, Direction dir) {
    if(anim == AnimationType::none) {return false;}
    if(anim == AnimationType::current) {anim = m_anim_state;}
    if(dir == Direction::current) {dir = m_direction;}
    if(m_animations.find(anim) == m_animations.end()) {
        std::cerr << "Animation state " << static_cast<int>(anim) << " for actor " << m_name << " is not defined!\n";
        return false;
    }
    if(m_animations[anim].find(dir) == m_animations[anim].end()) {
        std::cerr << "Direction" << static_cast<int>(dir) << " for animation state " << static_cast<int>(anim) << " of actor " << m_name << " is not defined!\n";
        return false;
    }
    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        m_animations[m_anim_state][m_direction].init_anim();
    }
    return m_animations[m_anim_state][m_direction].push_anim();
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c AnimSignal which indicates if the animation finished a cycle or hit its trigger
 */
AnimSignal Actor::animate_trigger(AnimationType anim, Direction dir) {
    if(anim == AnimationType::none) {return AnimSignal::none;}
    if(anim == AnimationType::current) {anim = m_anim_state;}
    if(dir == Direction::current) {dir = m_direction;}
    if(m_animations.find(anim) == m_animations.end()) {
        std::cerr << "Animation state " << static_cast<int>(anim) << " for actor " << m_name << " is not defined!\n";
        return AnimSignal::missing;
    }
    if(m_animations[anim].find(dir) == m_animations[anim].end()) {
        std::cerr << "Direction" << static_cast<int>(dir) << " for animation state " << static_cast<int>(anim) << " of actor " << m_name << " is not defined!\n";
        return AnimSignal::missing;
    }
    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        m_animations[m_anim_state][m_direction].init_anim();
    }
    return m_animations[m_anim_state][m_direction].push_anim_trigger();
}

/**
 * @brief Returns true if actor collides with rect
 * @note Hitbox width and height should be at least 10px
 *       when max actor speed is 500px per second
 * @param rect The rect against which collision gets checked
 * @param x_depth, y_depth The minimum x and y values to go back to not intersect anymore
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool Actor::collide(const SDL_Rect* rect, int& x_depth, int& y_depth, std::string type) const{
    SDL_Rect temp = get_hitbox(type);
    if(SDL_RectEmpty(&temp)) {return false;}
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

/**
 * @brief Returns true if actor collides with rect
 * @note Hitbox width and height should be at least 10px
 *       when max actor speed is 500px per second
 * @param rect The rect against which collision gets checked
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool Actor::collide(const SDL_Rect* rect, std::string type) const{
    SDL_Rect temp = get_hitbox(type);
    if(SDL_RectEmpty(&temp)) {return false;}
    temp.x += static_cast<int>(m_x);
    temp.y += static_cast<int>(m_y) - m_height;
    if(SDL_HasIntersection(&temp, rect) && !SDL_RectEquals(&temp, rect)) {
        return true;
    }
    return false;
}

/**
 * @brief Triggers event bound to Response value
 * @param r the Response value
 * @param cause Pointer to the Actor which may caused this event
 * @param key Keypress which may caused this event
 * @return @c bool indication if response is defined/gets triggered
 */
bool Actor::respond(Response r, Actor* cause, SDL_Keysym key) {
    if(m_response.find(r) == m_response.end()) {
        return false;
    }
    else {
        ActorEvent* event = m_response.at(r)->copy();
        if(cause != nullptr) {event->set_cause(cause);}
        if(key.sym != SDLK_UNKNOWN) {event->set_key(key);}
        add_event(event);
        return true;
    }
}

/**
 * @brief Return if actors event pipeline is blocked for a specific event
 * @param name Name of the event or event type
 * @return @c bool indicating if event or event type is currently blocked
 */
bool Actor::is_blocked(std::string name) const {
    if(m_block.find(name) == m_block.end()) {
        return false;
    }
    else {
        return m_block.at(name);
    }
}

/**
 * @brief Return if actors event pipeline is blocked for a specific key
 * @param key The key which gets checked
 * @return @c bool indicating if key is currently blocked
 */
bool Actor::is_blocked(const SDL_Keysym& key) const {
    if(m_block_key.find(key.sym) == m_block_key.end()) {
        return false;
    }
    else {
        return m_block_key.at(key.sym);
    }
}

/**
 * @brief Return if actors event pipeline is on cooldown for a specific event
 * @param name Name of the event or event type
 * @return @c bool indicating if event or event type is currently on cooldown
 */
bool Actor::in_cooldown(std::string name) const {
    if(m_timestamp.find(name) == m_timestamp.end()) {
        return false;
    }
    if(m_timestamp.at(name) > SDL_GetTicks()) {
        return true;
    }
    else {return false;}
}

/**
 * @brief Returns the hitbox of the supplied type
 * @param type The supplied type
 * @return @c SDL_Rect The hitbox
 * @note If there is no valid hitbox an empty one gets returned
 */
SDL_Rect Actor::get_hitbox(std::string type) const {
    if(m_hitbox.find(type) == m_hitbox.end()) {
        std::cerr << "Could not find hitbox " << type << " for actor " << m_name << "\n";
        return SDL_Rect{0,0,0,0};
    }
    else{
        return m_hitbox.at(type);
    }
}

bool Actor::on_ground(Direction dir) const {
    SDL_Rect pos = get_hitbox();
    pos.x += get_x();
    pos.y += get_y() - get_h();
    SDL_Rect temp;
    if(dir == Direction::up) {
        temp.x = pos.x;
        temp.y =pos.y - 1;
        temp.w = pos.w;
        temp.h = 1;
    }
    else if(dir == Direction::down) {
        temp.x = pos.x;
        temp.y =pos.y + pos.h;
        temp.w = pos.w;
        temp.h = 1;
    }
    else if(dir == Direction::left) {
        temp.x = pos.x - 1;
        temp.y =pos.y;
        temp.w = 1;
        temp.h = pos.h;
    }
    else if(dir == Direction::right) {
        temp.x = pos.x + pos.w;
        temp.y =pos.y;
        temp.w = 1;
        temp.h = pos.h;
    }
    else {
        return false;
    }
    return m_map->collide(&temp);
}

/**
 * @brief Deletes all event with given name or type except one
 */
unsigned Actor::scrap_event(std::string name, ActorEvent* except) {
    unsigned counter = 0;
    for(unsigned i = 0; i < m_event_pipeline.size(); i++) {
        if(( m_event_pipeline[i]->get_type() == name || m_event_pipeline[i]->name() == name) && m_event_pipeline[i] != except) {
            m_event_pipeline[i]->kill();
            m_event_pipeline.erase(m_event_pipeline.begin() + i);
            i--;
            counter++;
        }
    }
    return counter;
}
