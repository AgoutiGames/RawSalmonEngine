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

#include <iostream>

#include "map/mapdata.hpp"

Actor::Actor(Uint16 tile_id, MapData* mapdata) : Actor::Actor(mapdata->get_actor_template(tile_id), mapdata)
{

}


Actor::Actor(const ActorTemplate& templ, MapData* mapdata) :
 m_map {mapdata},
 m_type {templ.template_name},
 m_direction {templ.direction},
 m_hitbox {templ.hitbox},
 m_animations {templ.animations},
 m_response {templ.response}
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
        eResult = m_map->parse_actor_properties(p_property, m_direction, m_response);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing actor properties for actor: " << m_name << "\n";
            return eResult;
        }
    }
    respond(Response::on_spawn);
    return XML_SUCCESS;
}

/**
 * @brief Render the actor at it's position relative to the camera position
 * @param x_cam, y_cam The coordinates of the upper left corner of the camera rect
 */
void Actor::render(int x_cam, int y_cam) const {
    SDL_Rect dest {static_cast<int>(m_x - x_cam), static_cast<int>(m_y - m_height - y_cam), static_cast<int>(m_width), static_cast<int>(m_height)};
    m_animations.at(m_anim_state).at(m_direction).render(dest);
}

bool Actor::unstuck() {
    int x_inter_depth = 0;
    int y_inter_depth = 0;
    if(m_map->get_layer_collection().collide_terrain(this, x_inter_depth, y_inter_depth, true)) {
        // Do stuff with the intersection depth
        if(x_inter_depth < y_inter_depth) {
            m_x += x_inter_depth;
            if(m_map->get_layer_collection().collide_terrain(this, false)) {
                m_x -= 2 * x_inter_depth;
            }
        }
        else {
            m_y += y_inter_depth;
            if(m_map->get_layer_collection().collide_terrain(this, false)) {
                m_y -= 2 * y_inter_depth;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief Move actor to a direction by float factors
 * @param x_factor, y_factor Which indicate direction and extent of movement
 * @return a @c bool which indicates collision
 * @todo Apply tile speed modifiers
 */
bool Actor::move(float x_factor, float y_factor) {

    // Somewhat hacky approach to fix runtime switch of actor hitboxes via animation
    unstuck();

    bool moved = true;
    // Move the Actor
    float x_step;
    float y_step;
    x_step = x_factor;
    y_step = y_factor;

    // Determine if it can collide/ has hitbox
    SDL_Rect temp = get_hitbox();
    if(!SDL_RectEmpty(&temp)) {

        // Apply position of actor to hitbox
        m_x += x_step;
        // Check for x_axis collision
        if(x_factor != 0) {
            // Check for x-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->get_layer_collection().collide_terrain(this, x_inter_depth, y_inter_depth, true)) {
                // Do stuff with the intersection depth
                if(x_factor < 0) {x_inter_depth = -x_inter_depth;}
                m_x -= x_inter_depth;
                moved = false;
            }
        }
        // Check for y_axis collision
        if(y_factor != 0){
            // Apply y movement
            m_y += y_step;
            // Check for y-axis collision
            int x_inter_depth = 0;
            int y_inter_depth = 0;
            if(m_map->get_layer_collection().collide_terrain(this, x_inter_depth, y_inter_depth, true)) {
                // Do stuff with the intersection depth
                if(y_factor < 0) {y_inter_depth = -y_inter_depth;}
                m_y -= y_inter_depth;
                moved = false;
            }
        }
    }
    else {
        m_x += x_step;
        m_y += y_step;
    }
    return moved;
}

/**
 * @brief Update the actor state
 * @return @c bool which returns true if actor is alive
 */
bool Actor::update() {
    respond(Response::on_always);
    EventSignal sig = m_events.process_events(*this);

    switch(sig) {
        case(EventSignal::end) : {respond(Response::on_idle); return true;}
        case(EventSignal::erase) : {return false;}
        default : {return true;}
    }
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
 * @return @c AnimSignal which indicates if the animation finished a cycle or hit its trigger frame
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
    SDL_Rect inter;
    if(SDL_IntersectRect(&temp, rect, &inter) && !SDL_RectEquals(&temp, rect)) {
        x_depth = inter.w;
        y_depth = inter.h;
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
    if(SDL_HasIntersection(&temp, rect) && !SDL_RectEquals(&temp, rect)) {
        return true;
    }
    return false;
}

/**
 * @brief Triggers event bound to Response value
 * @param r the Response value
 * @param c the Cause value which shows who triggered this response
 * @return @c bool indication if response is defined/gets triggered
 */
bool Actor::respond(Response r, Cause c) {
    if(!m_response.check_event(r)) {
        return false;
    }
    else {
        SmartEvent<Actor> event = m_response.get_event(r);
        event->set_cause(c);
        m_events.add_event(event);
        return true;
    }
}

/**
 * @brief Returns the hitbox of the supplied type
 * @param type The supplied type
 * @return @c SDL_Rect The hitbox
 * @note If there is no valid hitbox an empty one gets returned
 */
SDL_Rect Actor::get_hitbox(std::string type) const {
    SDL_Rect hitbox = m_animations.at(m_anim_state).at(m_direction).get_hitbox(type);
    if(SDL_RectEmpty(&hitbox)) {
        if(m_hitbox.find(type) == m_hitbox.end()) {
            std::cerr << "Could not find hitbox " << type << " for actor " << m_name << "\n";
            return SDL_Rect{0,0,0,0};
        }
        else{
            hitbox = m_hitbox.at(type);
            hitbox.x += get_x();
            hitbox.y += get_y() - get_h();
            return hitbox;
        }
    }
    else {
        hitbox.x += get_x();
        hitbox.y += get_y() - get_h();
        return hitbox;
    }
}

const std::map<std::string, SDL_Rect> Actor::get_hitboxes() const {
    std::map<std::string, SDL_Rect> hitboxes = m_hitbox;
    for(const auto& hitbox_pair: m_animations.at(m_anim_state).at(m_direction).get_hitboxes()) {
        hitboxes[hitbox_pair.first] = hitbox_pair.second;
    }
    return hitboxes;
}

/**
 * @brief Checks if the actor is standing on ground
 * @param dir The direction of gravity
 * @return @c bool which is True if the actor is on ground
 */
bool Actor::on_ground(Direction dir, int tolerance) const {
    SDL_Rect pos = get_hitbox();
    SDL_Rect temp;
    if(dir == Direction::up) {
        temp.x = pos.x;
        temp.y =pos.y - 1 - tolerance;
        temp.w = pos.w;
        temp.h = 1 + tolerance;
    }
    else if(dir == Direction::down) {
        temp.x = pos.x;
        temp.y =pos.y + pos.h;
        temp.w = pos.w;
        temp.h = 1 + tolerance;
    }
    else if(dir == Direction::left) {
        temp.x = pos.x - 1 - tolerance;
        temp.y =pos.y;
        temp.w = 1 + tolerance;
        temp.h = pos.h;
    }
    else if(dir == Direction::right) {
        temp.x = pos.x + pos.w;
        temp.y =pos.y;
        temp.w = 1 + tolerance;
        temp.h = pos.h;
    }
    else {
        return false;
    }
    return m_map->get_layer_collection().collide_terrain(temp);
}

