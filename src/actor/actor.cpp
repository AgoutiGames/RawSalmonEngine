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
#include "actor/actor.hpp"

#include <iostream>

#include "map/mapdata.hpp"
#include "util/parse.hpp"

Actor::Actor(MapData* map) : m_map{map} {}

/**
 * @brief Initialize actor dimensions and name from XML info
 * @param source The @c XMLElement which contains the information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Actor::parse_base(tinyxml2::XMLElement* source) {
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
    return XML_SUCCESS;
}

/**
 * @brief Initialize custom actor properties from XML info
 * @param source The @c XMLElement which contains the information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Actor::parse_properties(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties == nullptr) {
        // Missing properties are generally considered okay
        return XML_SUCCESS;
    }
    XMLElement* p_property = p_tile_properties->FirstChildElement("property");

    // Iterate over all property elements
    while(p_property != nullptr) {
        // Get name of property
        const char* p_name;
        p_name = p_property->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse type/template name of actor
        else if(name == "ACTOR_NAME") {
            const char* p_actor_name = p_property->Attribute("value");
            if(p_actor_name == nullptr) {
                std::cerr << "Empty actor name specified\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            m_type = p_actor_name;
        }

        // Parse current direction facing
        else if(name == "DIRECTION") {
            const char* p_direction = p_property->Attribute("value");
            if(p_direction != nullptr) {
                Direction dir = str_to_direction(std::string(p_direction));
                if(dir == Direction::invalid) {
                    std::cerr << "Invalid direction type \"" << p_direction << "\"specified\n";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }

                if(dir == Direction::current) {
                    std::cerr << "There is no current direction upon actor initialization\n";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }
                m_direction = dir;
            }
            else {
                std::cerr << "Empty direction value specified\n";
                return XML_NO_ATTRIBUTE;
            }

        }

        // Parse response values
        else if(str_to_response(name) != Response::invalid) {
            const char* p_event = p_property->Attribute("value");
            if(p_event != nullptr) {
                std::string event(p_event);
                if(m_map->check_event_convert_actor(event)) {
                    m_response.register_event(str_to_response(name), m_map->get_event_convert_actor(event));
                }
                else {
                    std::cerr << "An event called: " << event << " does not exist/ never got parsed!\n";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }
            else {
                std::cerr << "Empty response event specified\n";
                return XML_NO_ATTRIBUTE;
            }
        }

        else {
            std::cerr << "Unknown actor property \"" << p_name << "\" specified\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        // Move to next property
        p_property = p_property->NextSiblingElement("property");
    }
    return XML_SUCCESS;
}

/**
 * @brief Initialize actors hitboxes from XML info
 * @param source The @c XMLElement which contains the information
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Actor::parse_hitbox(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    // Parse the default hitbox
    XMLElement* p_objgroup = source->FirstChildElement("objectgroup");
    if(p_objgroup != nullptr) {
        XMLElement* p_object = p_objgroup->FirstChildElement("object");
        if(p_object != nullptr) {
            eResult = parse::hitboxes(p_object, m_hitbox);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at parsing hitbox for actor template: " << m_type << "\n";
                return eResult;
            }
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
    m_animations.at(m_anim_state).at(m_direction).render(dest);
}

/**
 * @brief Checks if actor collides with tiles and moves it away
 * @note This approach has to check for collision twice because we don't know if the actor is
 *       left or right/ontop or below the tile. We try either way and check for collision again.
 *       With another collide function providing this information we could increase the performance.
 */
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
 *
 * The algorithm applies the movement seperately for each axis and thus checks twice for collision.
 * This approach is pretty bugged if actor is already stuck in another hitbox.
 * The only way to solve this is to additionally check for collision and move away before the actual movement via unstuck()
 */
bool Actor::move(float x_factor, float y_factor, bool absolute) {

    // Somewhat hacky approach to fix runtime switch of actor hitboxes via animation
    unstuck();

    bool moved = true;
    // Move the Actor
    float x_step;
    float y_step;
    x_step = x_factor;
    y_step = y_factor;

    // Position actor exactly at the word coords of x and y
    if(absolute) {
        x_step = x_factor - m_x;
        y_step = y_factor - m_y;
    }

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
 */
void Actor::update() {
    if(m_first) {
        m_first = false;
        respond(Response::on_spawn);
    }
    respond(Response::on_always);
    EventSignal sig = m_events.process_events(*this);

    if(sig == EventSignal::end) {
        respond(Response::on_idle);
    }
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c bool which indicates if the animation finished a cycle/wrapped around
 */
bool Actor::animate(AnimationType anim, Direction dir, float speed) {
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
    return m_animations[m_anim_state][m_direction].push_anim(speed);
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c AnimSignal which indicates if the animation finished a cycle or hit its trigger frame
 */
AnimSignal Actor::animate_trigger(AnimationType anim, Direction dir, float speed) {
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
    return m_animations[m_anim_state][m_direction].push_anim_trigger(speed);
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
 * @brief Returns the active hitbox of the supplied type
 * @param type The supplied type
 * @return @c SDL_Rect The hitbox
 * @note If there is no valid hitbox an empty one gets returned
 *
 * First the actor checks if the active animation has the hitbox with the name
 * and returns it instead.
 */
SDL_Rect Actor::get_hitbox(std::string type) const {
    SDL_Rect hitbox = m_animations.at(m_anim_state).at(m_direction).get_hitbox(type);
    if(SDL_RectEmpty(&hitbox)) {
        if(m_hitbox.find(type) == m_hitbox.end()) {
            // std::cerr << "Could not find hitbox " << type << " for actor " << m_name << "\n";
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

/**
 * @brief Returns all active hitboxes
 *
 * To the hitboxes of the actor tile, possible hitboxes of the active
 * animation and its animation frame are added. Specific ones may override general ones.
 */
const std::map<std::string, SDL_Rect> Actor::get_hitboxes() const {
    std::map<std::string, SDL_Rect> hitboxes = m_hitbox;
    for(const auto& hitbox_pair: m_animations.at(m_anim_state).at(m_direction).get_hitboxes()) {
        hitboxes[hitbox_pair.first] = hitbox_pair.second;
    }
    for(auto& hitbox_pair : hitboxes) {
        hitbox_pair.second.x += get_x();
        hitbox_pair.second.y += get_y() - get_h();
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

