/*
 * Copyright 2017-2020 Agouti Games Team (see the AUTHORS file)
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
#include "map/map_layer.hpp"
#include "map/object_layer.hpp"
#include "util/logger.hpp"
#include "util/parse.hpp"

using namespace salmon;

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
    unsigned individual_width, individual_height;
    eResult = source->QueryUnsignedAttribute("width", &individual_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryUnsignedAttribute("height", &individual_height);
    if(eResult != XML_SUCCESS) return eResult;

    // If a custom scale is supplied, scale accordingly and adjust position to match
    if(individual_width != m_width) {
        m_x_scale = static_cast<float>(individual_width) / m_width;
        m_x += (m_x_scale - 1.0f) * m_width * 0.5f;
        m_scaled = true;
    }
    if(individual_height != m_height) {
        m_y_scale = static_cast<float>(individual_height) / m_height;
        m_y += (m_y_scale - 1.0f) * m_height * 0.5f;
        m_scaled = true;
    }
    // Parse the (unique) name of the actor
    const char* p_actor_name;
    p_actor_name = source->Attribute("name");
    if(p_actor_name == nullptr) {
        Logger(Logger::error) << "Actor at x: " << m_x << " y: " << m_y << " is missing a custom name!";
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
                Logger(Logger::error) << "Empty actor name specified";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            m_type = p_actor_name;
        }

        // Parse current direction facing
        else if(name == "DIRECTION") {
            const char* p_direction = p_property->Attribute("value");
            if(p_direction != nullptr) {
                salmon::Direction dir = str_to_direction(std::string(p_direction));
                if(dir == salmon::Direction::invalid) {
                    Logger(Logger::error) << "Invalid direction type \"" << p_direction << "\"specified";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }

                if(dir == salmon::Direction::current) {
                    Logger(Logger::error) << "There is no current direction upon actor initialization";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }
                m_direction = dir;
            }
            else {
                Logger(Logger::error) << "Empty direction value specified";
                return XML_NO_ATTRIBUTE;
            }

        }

        // Parse animation type
        else if(name == "ANIMATION_TYPE") {
            const char* p_anim_type = p_property->Attribute("value");
            if(p_anim_type != nullptr) {
                std::string anim = p_anim_type;
                if(anim == salmon::AnimationType::current) {
                    Logger(Logger::error) << "You can't define a specific animation type as the current one";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }
                m_anim_state = anim;
            }
            else {
                Logger(Logger::error) << "Missing animation type";
                return XML_NO_ATTRIBUTE;
            }
        }

        else if(name == "LATE_POLLING") {
            XMLError eResult = p_property->QueryBoolAttribute("value", &m_late_polling);
            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Failed parsing the LATE_POLLING property";
                return eResult;
            }
        }

        #ifndef LIB_BUILD
            // Parse response values
            else if(str_to_response(name) != Response::invalid) {
                const char* p_event = p_property->Attribute("value");
                if(p_event != nullptr) {
                    std::string event(p_event);
                    if(m_map->check_event_convert_actor(event)) {
                        m_response.register_event(str_to_response(name), m_map->get_event_convert_actor(event));
                    }
                    else {
                        Logger(Logger::error) << "An event called: " << event << " does not exist/ never got parsed!";
                        return XML_ERROR_PARSING_ATTRIBUTE;
                    }
                }
                else {
                    Logger(Logger::error) << "Empty response event specified";
                    return XML_NO_ATTRIBUTE;
                }
            }
        #endif // LIB_BUILD

        else {
            XMLError eResult;
            const char* p_type = p_property->Attribute("type");
            std::string type = (p_type) ? p_type : "";
            if(type == "bool") {
                bool temp;
                eResult = p_property->QueryBoolAttribute("value", &temp);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Malformed bool property: " << name;
                    return eResult;
                }
                m_data.set_val(name, temp);
            }
            else if(type == "int") {
                int temp;
                eResult = p_property->QueryIntAttribute("value", &temp);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Malformed int property: " << name;
                    return eResult;
                }
                m_data.set_val(name, temp);
            }
            else if(type == "float") {
                float temp;
                eResult = p_property->QueryFloatAttribute("value", &temp);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Malformed float property: " << name;
                    return eResult;
                }
                m_data.set_val(name, temp);
            }
            else if(type == "" || type == "file") {
                const char* p_value = p_property->Attribute("value");
                if(p_value == nullptr) {
                    Logger(Logger::error) << "Malformed string property: " << name;
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
                std::string value = "";
                if(type == "file") {value += m_map->get_file_path();}
                value += p_value;
                m_data.set_val(name, value);
            }
            else {
                Logger(Logger::error) << "Unknown type " << type << " specified! This shouldn't happen at all! Tiled must have messed up";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            /*
            Logger(Logger::error) << "Unknown actor property \"" << p_name << "\" specified";
            return XML_ERROR_PARSING_ATTRIBUTE;
            */
        }
        // Move to next property
        p_property = p_property->NextSiblingElement("property");
    }
    return XML_SUCCESS;
}

bool Actor::scale(float x, float y) {
    bool success = true;
    if(x > 0.0f) {m_x_scale = x;}
    else {success = false;}
    if(y > 0.0f) {m_y_scale = y;}
    else {success = false;}

    constexpr float tolerance = 0.001f;
    if(m_x_scale > 1.0f + tolerance || m_x_scale < 1.0f - tolerance || m_y_scale > 1.0f + tolerance || m_y_scale < 1.0f - tolerance) {
        m_scaled = true;
    }
    else {
        m_scaled = false;
    }
    return success;
}

/**
 * @brief Render the actor at it's position relative to the camera position
 * @param x_cam, y_cam The coordinates of the upper left corner of the camera rect
 */
void Actor::render(int x_cam, int y_cam) const {
    if(m_hidden) {return;}
    const Tile* current_tile = nullptr;
    if(m_anim_state != salmon::AnimationType::none && valid_anim_state()) {current_tile = &m_animations.at(m_anim_state).at(m_direction);}
    else {current_tile = &m_base_tile;}

    if(m_scaled) {
        float current_x = m_x - (m_x_scale - 1.0f) * m_width * 0.5f;
        float current_y = m_y - (m_y_scale - 1.0f) * m_height * 0.5f;
        unsigned current_h = m_height * m_y_scale;
        unsigned current_w = m_width * m_x_scale;
        if(m_angle > 0.1 || m_angle < -0.1) {
            SDL_Rect dest {static_cast<int>(current_x - x_cam), static_cast<int>(current_y - current_h - y_cam), static_cast<int>(current_w), static_cast<int>(current_h)};
            current_tile->render_extra(dest, m_angle, false, false);
        }
        else {
            SDL_Rect dest {static_cast<int>(current_x - x_cam), static_cast<int>(current_y - current_h - y_cam), static_cast<int>(current_w), static_cast<int>(current_h)};
            current_tile->render(dest);
        }
    }
    else {
        if(m_angle > 0.1 || m_angle < -0.1) {
            SDL_Rect dest {static_cast<int>(m_x - x_cam), static_cast<int>(m_y - m_height - y_cam), static_cast<int>(m_width), static_cast<int>(m_height)};
            current_tile->render_extra(dest, m_angle, false, false);
        }
        else {
            SDL_Rect dest {static_cast<int>(m_x - x_cam), static_cast<int>(m_y - m_height - y_cam), static_cast<int>(m_width), static_cast<int>(m_height)};
            current_tile->render(dest);
        }
    }
}

float Actor::get_bottom() const {
    if(m_scaled) {
        return get_y() - (m_y_scale - 1.0f) * m_height * 0.5f;
    }
    else {
        return get_y();
    }
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

    unstuck(salmon::Collidees::tile,{salmon::DEFAULT_HITBOX},{salmon::DEFAULT_HITBOX},true);
    if(absolute) {
        return move_absolute(x_factor,y_factor,salmon::Collidees::tile,{salmon::DEFAULT_HITBOX},{salmon::DEFAULT_HITBOX},true);
    }
    else {
        return move_relative(x_factor,y_factor,salmon::Collidees::tile,{salmon::DEFAULT_HITBOX},{salmon::DEFAULT_HITBOX},true);
    }
}

bool Actor::move_relative(float x, float y, salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    move_relative(x,y);
    normalize(x,y);
    return !unstuck_along_path(-x,-y,target,my_hitboxes,other_hitboxes,notify);
}
bool Actor::move_absolute(float x, float y, salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    move_absolute(x,y);
    normalize(x,y);
    return !unstuck_along_path(-x,-y,target,my_hitboxes,other_hitboxes,notify);
}

void Actor::move_relative(float x, float y) {
    m_x += x;
    m_y += y;
}
void Actor::move_absolute(float x, float y) {
    m_x = x;
    m_y = y;
}

bool Actor::unstuck(salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    LayerCollection& layer_collection = get_map().get_layer_collection();
    SDL_Rect bounds = get_boundary();
    bool moved = false;
    if(target == salmon::Collidees::tile || target == salmon::Collidees::tile_and_actor) {
        for(MapLayer* map : layer_collection.get_map_layers()) {
            for(TileInstance& tile : map->get_clip(bounds)) {
                if(separate(tile,my_hitboxes,other_hitboxes,notify)) {
                    moved = true;
                }
            }
        }
    }
    if(target == salmon::Collidees::actor || target == salmon::Collidees::tile_and_actor) {
        for(ObjectLayer* obj : layer_collection.get_object_layers()) {
            for(Actor* actor : obj->get_clip(bounds)) {
                if(separate(*actor,my_hitboxes,other_hitboxes, notify)) {
                    moved = true;
                }
            }
        }
    }
    return moved;
}

bool Actor::unstuck_along_path(float x, float y,salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    LayerCollection& layer_collection = get_map().get_layer_collection();
    SDL_Rect bounds = get_boundary();
    bool moved = false;
    if(target == salmon::Collidees::tile || target == salmon::Collidees::tile_and_actor) {
        for(MapLayer* map : layer_collection.get_map_layers()) {
            for(TileInstance& tile : map->get_clip(bounds)) {
                if(separate_along_path(x,y,tile,my_hitboxes,other_hitboxes,notify)) {
                    moved = true;
                }
            }
        }
    }
    if(target == salmon::Collidees::actor || target == salmon::Collidees::tile_and_actor) {
        std::vector<Actor*> actors;
        for(ObjectLayer* obj : layer_collection.get_object_layers()) {
            std::vector<Actor*> new_actors = obj->get_clip(bounds);
            actors.insert(actors.end(),new_actors.begin(),new_actors.end());
        }
        if(actors.empty()) {return moved;}
        for(Actor* actor : actors) {
            if(actor == this) {continue;}
            if(separate_along_path(x,y,*actor,my_hitboxes,other_hitboxes, notify)) {
                moved = true;
            }
        }
    }
    return moved;
}

#ifndef LIB_BUILD
    /**
     * @brief Update the actor state
     */
    void Actor::update() {

        respond(Response::on_always);

        for(Collision c : m_collisions) {
            if(c.mouse()) {respond(Response::on_mouse, c);}
            else if(!c.none()) {respond(Response::on_collision, c);}
            else {
                Logger(Logger::error) << "Actor: " << m_name << " tried to respond to \"empty\" collision. This shouldn't happen!";
            }
        }
        clear_collisions();

        EventSignal sig = m_events.process_events(*this);

        if(sig == EventSignal::end) {
            respond(Response::on_idle);
        }
    }
#endif // LIB_BUILD

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c bool which indicates if the animation finished a cycle/wrapped around
 */
bool Actor::animate(std::string anim, salmon::Direction dir, float speed) {
    if(anim == salmon::AnimationType::current) {anim = m_anim_state;}
    if(dir == salmon::Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == salmon::AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return false;}

    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        current_tile->init_anim();
    }

    return current_tile->push_anim(speed);
}

/**
 * @brief Set animation tile to specific frame
 */
bool Actor::set_animation(std::string anim, salmon::Direction dir, int frame) {
    if(anim == salmon::AnimationType::current) {anim = m_anim_state;}
    if(dir == salmon::Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == salmon::AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return false;}

    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        current_tile->init_anim();
    }

    return current_tile->set_frame(frame);
}

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c AnimSignal which indicates if the animation finished a cycle or hit its trigger frame
 */
AnimSignal Actor::animate_trigger(std::string anim, salmon::Direction dir, float speed) {
    if(anim == salmon::AnimationType::current) {anim = m_anim_state;}
    if(dir == salmon::Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == salmon::AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return AnimSignal::missing;}

    if(m_anim_state != anim || m_direction != dir) {
        m_anim_state = anim;
        m_direction = dir;
        current_tile->init_anim();
    }

    return current_tile->push_anim_trigger(speed);
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

#ifndef LIB_BUILD
    /**
     * @brief Triggers event bound to Response value
     * @param r the Response value
     * @param c the Cause value which shows who triggered this response
     * @return @c bool indication if response is defined/gets triggered
     */
    bool Actor::respond(Response r, Collision c) {
        if(!m_response.check_event(r)) {
            return false;
        }
        else {
            SmartEvent<Actor> event = m_response.get_event(r);
            event->set_collision(c);
            m_events.add_event(event);
            return true;
        }
    }
#endif // LIB_BUILD

/// Checks if the currently set animation state and direction are existin
bool Actor::valid_anim_state(std::string anim, salmon::Direction dir) const {
    //if(m_anim_state == AnimationType::none) {return true;}
    if(m_animations.find(anim) == m_animations.end()) {
        Logger(Logger::error) << "Animation state " << anim << " for actor " << m_name << " is not defined!";
        return false;
    }
    if(m_animations.at(anim).find(dir) == m_animations.at(anim).end()) {
        Logger(Logger::error) << "Direction" << static_cast<int>(dir) << " for animation state " << anim << " of actor " << m_name << " is not defined!";
        return false;
    }
    return true;
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


    SDL_Rect current_hitbox = {0,0,0,0};
    // Try extracting hitbox from currenty active animated tile
    if(m_anim_state != salmon::AnimationType::none && valid_anim_state()) {
        current_hitbox = m_animations.at(m_anim_state).at(m_direction).get_hitbox(type);
    }
    // If that failed, extract hitbox from base actor tile
    if(SDL_RectEmpty((&current_hitbox))) {current_hitbox = m_base_tile.get_hitbox(type);}
    // If that also failed just return the empty hitbox
    if(SDL_RectEmpty((&current_hitbox))) {return current_hitbox;}

    // Otherwise adjust hitbox position and return
    transform_hitbox(current_hitbox);

    return current_hitbox;
}

/**
 * @brief Returns all active hitboxes
 *
 * To the hitboxes of the actor tile, possible hitboxes of the active
 * animation and its animation frame are added. Specific ones may override general ones.
 */
const std::map<std::string, SDL_Rect> Actor::get_hitboxes() const {
    // Get all hitboxes from base tile
    std::map<std::string, SDL_Rect> hitboxes = m_base_tile.get_hitboxes();
    // If there is a valid animation tile, load those "ontop" of the other hitboxes
    if(m_anim_state != salmon::AnimationType::none && valid_anim_state()) {
        for(const auto& hitbox_pair: m_animations.at(m_anim_state).at(m_direction).get_hitboxes()) {
            hitboxes[hitbox_pair.first] = hitbox_pair.second;
        }
    }
    // Adjust each hitbox position and return
    for(auto& hitbox_pair : hitboxes) {
        transform_hitbox(hitbox_pair.second);
    }
    return hitboxes;
}

void Actor::transform_hitbox(SDL_Rect& hitbox) const {
    if(m_resize_hitbox && m_scaled) {

        hitbox.y -= static_cast<int>(get_h());
        hitbox.y *= m_y_scale;
        hitbox.h *= m_y_scale;
        hitbox.y -= (m_y_scale - 1.0f) * get_h() / 2;
        hitbox.y += get_y();

        hitbox.x *= m_x_scale;
        hitbox.w *= m_x_scale;
        hitbox.x -= (m_x_scale - 1.0f) * get_w() / 2;
        hitbox.x += get_x();
    }
    else {
        hitbox.x += get_x();
        hitbox.y += get_y() - static_cast<int>(get_h());
    }
}

/**
 * @brief Checks if the actor is standing on ground
 * @param dir The direction of gravity
 * @return @c bool which is True if the actor is on ground
 */
bool Actor::on_ground(salmon::Direction dir, int tolerance) const {
    SDL_Rect pos = get_hitbox();
    SDL_Rect temp;
    if(dir == salmon::Direction::up) {
        temp.x = pos.x;
        temp.y =pos.y - 1 - tolerance;
        temp.w = pos.w;
        temp.h = 1 + tolerance;
    }
    else if(dir == salmon::Direction::down) {
        temp.x = pos.x;
        temp.y =pos.y + pos.h;
        temp.w = pos.w;
        temp.h = 1 + tolerance;
    }
    else if(dir == salmon::Direction::left) {
        temp.x = pos.x - 1 - tolerance;
        temp.y =pos.y;
        temp.w = 1 + tolerance;
        temp.h = pos.h;
    }
    else if(dir == salmon::Direction::right) {
        temp.x = pos.x + pos.w;
        temp.y =pos.y;
        temp.w = 1 + tolerance;
        temp.h = pos.h;
    }
    else {
        return false;
    }
    return m_map->get_layer_collection().check_collision(temp, salmon::Collidees::tile,{salmon::DEFAULT_HITBOX});
}

bool Actor::separate(TileInstance& tile, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = tile.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(separate(first_hitbox, second_hitbox)) {
                moved = true;
                if(notify) {
                    add_collision({tile,first_hitbox_name,second_hitbox_name});
                }
            }
        }
    }
    return moved;
}

bool Actor::separate(Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = actor.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(separate(first_hitbox, second_hitbox)) {
                moved = true;
                if(notify) {
                    add_collision({&actor,first_hitbox_name,second_hitbox_name});
                    actor.add_collision({this,second_hitbox_name,first_hitbox_name});
                }
            }
        }
    }
    return moved;
}

bool Actor::separate(const SDL_Rect& first, const SDL_Rect& second) {
    SDL_Rect intersect;
    if(!SDL_IntersectRect(&first, &second, &intersect)) {return false;}
    SDL_Point delta = rect_center_difference(first,second);

    // Move in y direction
    if(intersect.w > intersect.h) {
        // first is above second
        if(delta.y > 0) {
            move_relative(0,-intersect.h);
        }
        // first is below second
        else {
            move_relative(0,intersect.h);
        }
    }
    // Move in x direction
    else {
        // first is left of second
        if(delta.x > 0) {
            move_relative(-intersect.w,0);
        }
        // first is right of second
        else {
            move_relative(intersect.w,0);
        }
    }

    return true;
}

bool Actor::separate_along_path(float x, float y,TileInstance& tile, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = tile.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(separate_along_path(x, y,first_hitbox, second_hitbox)) {
                moved = true;
                if(notify) {
                    add_collision({tile,first_hitbox_name,second_hitbox_name});
                }
            }
        }
    }
    return moved;
}

bool Actor::separate_along_path(float x, float y,Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = actor.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(separate_along_path(x, y,first_hitbox, second_hitbox)) {
                moved = true;
                if(notify) {
                    add_collision({&actor,first_hitbox_name,second_hitbox_name});
                    actor.add_collision({this,second_hitbox_name,first_hitbox_name});
                }
            }
        }
    }
    return moved;
}


bool Actor::separate_along_path(float x, float y,const SDL_Rect& first, const SDL_Rect& second) {
    if(!SDL_HasIntersection(&first, &second)) {return false;}
    if(x == 0.0) {
        // No direction value means separate via shortest distance
        if(y == 0.0) {return separate(first,second);}
        // Move down
        if(y > 0.0) {
            // Move by distance of firsts upper ledge and seconds lower ledge
            move_relative(0,second.y + second.h - first.y);
        }
        // Move up
        else {
            // Move by distance of firsts lower ledge and seconds upper ledge
            move_relative(0, -(first.y + first.h - second.y) );
        }
    }
    else if(y == 0.0) {
        // Move right
        if(x > 0.0) {
            // Move by distance of firsts left ledge and seconds right ledge
            move_relative(second.x + second.w - first.x,0);
        }
        // Move left
        else {
            // Move by distance of firsts right ledge and seconds left ledge
            move_relative(-(first.x + first.w - second.x) ,0);
        }
    }
    else {
        if(x > 0.0) {
            // Move Right Down
            if(y > 0.0) {
                SDL_Point upper_left_corner = {first.x,first.y};
                int right_dist = second.x + second.w - upper_left_corner.x;
                int down_dist = second.y + second.h - upper_left_corner.y;

                float x_steps = right_dist / std::abs(x);
                float y_steps = down_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(right_dist,static_cast<int>(x_steps * y));
                }
                else {
                    move_relative(static_cast<int>(y_steps * x),down_dist);
                }
            }
            // Move Right Up
            else {
                SDL_Point lower_left_corner = {first.x,first.y + first.h};
                int right_dist = second.x + second.w - lower_left_corner.x;
                int up_dist = lower_left_corner.y - second.y;

                float x_steps = right_dist / std::abs(x);
                float y_steps = up_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(right_dist, static_cast<int>(x_steps * y));
                }
                else {
                    move_relative(static_cast<int>(y_steps * x),-up_dist);
                }
            }
        }
        else {
            // Move Left Down
            if(y > 0.0) {
                SDL_Point upper_right_corner = {first.x+first.w,first.y};
                int left_dist = upper_right_corner.x - second.x;
                int down_dist = second.y + second.h - upper_right_corner.y;

                float x_steps = left_dist / std::abs(x);
                float y_steps = down_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(-left_dist, static_cast<int>(x_steps * y));
                }
                else {
                    move_relative(static_cast<int>(y_steps * x),down_dist);
                }
            }
            // Move Left Up
            else {
                SDL_Point lower_right_corner = {first.x+first.w,first.y+first.h};
                int left_dist = lower_right_corner.x - second.x;
                int up_dist = lower_right_corner.y - second.y;

                float x_steps = left_dist / std::abs(x);
                float y_steps = up_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(-left_dist, static_cast<int>(x_steps * y));
                }
                else {
                    move_relative(static_cast<int>(y_steps * x),-up_dist);
                }
            }
        }
    }
    return true;
}

bool Actor::check_collision(Actor& other, bool notify) {
    bool collided = false;
    for(auto first_hitbox : get_hitboxes()) {
        SDL_Rect& first = first_hitbox.second;
        if(SDL_RectEmpty(&first)) {continue;}
        for(auto second_hitbox : other.get_hitboxes()) {
            SDL_Rect& second = second_hitbox.second;
            if(SDL_RectEmpty(&second)) {continue;}
            if(SDL_HasIntersection(&first,&second)) {
                collided = true;
                if(notify) {
                    add_collision({&other,first_hitbox.first,second_hitbox.first});
                    other.add_collision({this,second_hitbox.first,first_hitbox.first});
                }
            }
        }
    }
    return collided;
}
bool Actor::check_collision(Actor& other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool collided = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = other.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(SDL_HasIntersection(&first_hitbox,&second_hitbox)) {
                collided = true;
                if(notify) {
                    add_collision({&other,first_hitbox_name,second_hitbox_name});
                    other.add_collision({this,second_hitbox_name,first_hitbox_name});
                }
            }
        }
    }
    return collided;
}

bool Actor::check_collision(TileInstance& other, bool notify) {
    bool collided = false;
    for(auto first_hitbox : get_hitboxes()) {
        SDL_Rect& first = first_hitbox.second;
        if(SDL_RectEmpty(&first)) {continue;}
        for(auto second_hitbox : other.get_hitboxes()) {
            SDL_Rect& second = second_hitbox.second;
            if(SDL_RectEmpty(&second)) {continue;}
            if(SDL_HasIntersection(&first,&second)) {
                collided = true;
                if(notify) {
                    add_collision({other,first_hitbox.first,second_hitbox.first});
                }
            }
        }
    }
    return collided;
}
bool Actor::check_collision(TileInstance& other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool collided = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        SDL_Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(SDL_RectEmpty(&first_hitbox)) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            SDL_Rect second_hitbox = other.get_hitbox(second_hitbox_name);
            if(SDL_RectEmpty(&second_hitbox)) {continue;}
            if(SDL_HasIntersection(&first_hitbox,&second_hitbox)) {
                collided = true;
                if(notify) {
                    add_collision({other,first_hitbox_name,second_hitbox_name});
                }
            }
        }
    }
    return collided;
}
