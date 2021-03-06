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
#include "types.hpp"

namespace salmon { namespace internal {

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
    float x_pos, y_pos;
    eResult = source->QueryFloatAttribute("x", &x_pos);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryFloatAttribute("y", &y_pos);
    if(eResult != XML_SUCCESS) return eResult;
    float individual_width, individual_height;
    eResult = source->QueryFloatAttribute("width", &individual_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = source->QueryFloatAttribute("height", &individual_height);
    if(eResult != XML_SUCCESS) return eResult;

    m_transform.set_pos(x_pos,y_pos);

    double rotation = 0.0;
    eResult = source->QueryDoubleAttribute("rotation", &rotation);
    if(rotation != 0.0) {
        m_transform.set_rotation(rotation);
    }

    // If a custom scale is supplied, scale accordingly and adjust position to match
    auto base_dimensions = m_transform.get_base_dimensions();
    if(individual_width != base_dimensions.w || individual_height != base_dimensions.h) {
        m_transform.set_scale(individual_width / base_dimensions.w, individual_height / base_dimensions.h);
    }

    // Parse the (unique) name of the actor
    const char* p_actor_name;
    p_actor_name = source->Attribute("name");
    if(p_actor_name == nullptr) {
        Logger(Logger::error) << "Actor at x: " << x_pos << " y: " << y_pos << " is missing a custom name!";
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
                Direction dir = str_to_direction(std::string(p_direction));
                if(dir == Direction::invalid) {
                    Logger(Logger::error) << "Invalid direction type \"" << p_direction << "\"specified";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }

                if(dir == Direction::current) {
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
                if(anim == AnimationType::current) {
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

/**
 * @brief Render the actor at it's position relative to the camera position
 * @param x_cam, y_cam The coordinates of the upper left corner of the camera rect
 */
void Actor::render(float x_cam, float y_cam) const {
    if(m_hidden) {return;}
    const Tile* current_tile = nullptr;
    if(m_anim_state != AnimationType::none && valid_anim_state()) {current_tile = &m_animations.at(m_anim_state).at(m_direction);}
    else {current_tile = &m_base_tile;}

    Rect dest = m_transform.to_rect();
    dest.x -= x_cam;
    dest.y -= y_cam;

    if(m_transform.is_rotated() || m_transform.is_flipped()) {
        double rotation = m_transform.get_rotation();
        auto rot = m_transform.get_rotation_center();
        current_tile->render_extra(dest, rotation, m_transform.get_h_flip(), m_transform.get_v_flip(), rot.x, rot.y);
    }
    else {
        current_tile->render(dest);
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

    unstuck(Collidees::tile,{DEFAULT_HITBOX},{DEFAULT_HITBOX},true);
    if(absolute) {
        return move_absolute(x_factor,y_factor,Collidees::tile,{DEFAULT_HITBOX},{DEFAULT_HITBOX},true);
    }
    else {
        return move_relative(x_factor,y_factor,Collidees::tile,{DEFAULT_HITBOX},{DEFAULT_HITBOX},true);
    }
}

bool Actor::move_relative(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    move_relative(x,y);
    return !unstuck_along_path(-x,-y,target,my_hitboxes,other_hitboxes,notify);
}
bool Actor::move_absolute(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    move_absolute(x,y);
    return !unstuck(target,my_hitboxes,other_hitboxes,notify);
}

void Actor::move_relative(float x, float y) {
    m_transform.move_pos(x,y);
}
void Actor::move_absolute(float x, float y) {
    m_transform.set_pos(x,y);
}

bool Actor::unstuck(Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    LayerCollection& layer_collection = get_map().get_layer_collection();
    Rect bounds = m_transform.to_bounding_box();
    bool moved = false;
    if(target == Collidees::tile || target == Collidees::tile_and_actor) {
        for(MapLayer* map : layer_collection.get_map_layers()) {
            for(TileInstance& tile : map->get_clip(bounds)) {
                if(separate(tile,my_hitboxes,other_hitboxes,notify)) {
                    moved = true;
                }
            }
        }
    }
    if(target == Collidees::actor || target == Collidees::tile_and_actor) {
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

bool Actor::unstuck_along_path(float x, float y,Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    LayerCollection& layer_collection = get_map().get_layer_collection();
    Rect bounds = m_transform.to_bounding_box();
    bool moved = false;
    if(target == Collidees::tile || target == Collidees::tile_and_actor) {
        for(MapLayer* map : layer_collection.get_map_layers()) {
            for(TileInstance& tile : map->get_clip(bounds)) {
                if(separate_along_path(x,y,tile,my_hitboxes,other_hitboxes,notify)) {
                    moved = true;
                }
            }
        }
    }
    if(target == Collidees::actor || target == Collidees::tile_and_actor) {
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

/**
 * @brief Animate the actor
 * @param anim The type of the animation
 * @param dir The direction of the animation
 * @return @c bool which indicates if the animation finished a cycle/wrapped around
 */
bool Actor::animate(std::string anim, Direction dir, float speed) {
    if(anim == AnimationType::current) {anim = m_anim_state;}
    if(dir == Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return false;}

    if(m_anim_state != anim || m_direction != dir) {
        // Set rendering dimensions to current tile
        m_transform.set_dimensions(current_tile->get_w(), current_tile->get_h());
        m_anim_state = anim;
        m_direction = dir;
        current_tile->init_anim();
    }

    return current_tile->push_anim(speed);
}

/**
 * @brief Set animation tile to specific frame
 */
bool Actor::set_animation(std::string anim, Direction dir, int frame) {
    if(anim == AnimationType::current) {anim = m_anim_state;}
    if(dir == Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return false;}

    if(m_anim_state != anim || m_direction != dir) {
        // Set rendering dimensions to current tile
        m_transform.set_dimensions(current_tile->get_w(), current_tile->get_h());
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
AnimSignal Actor::animate_trigger(std::string anim, Direction dir, float speed) {
    if(anim == AnimationType::current) {anim = m_anim_state;}
    if(dir == Direction::current) {dir = m_direction;}

    Tile* current_tile = nullptr;
    if(anim == AnimationType::none) {current_tile = &m_base_tile;}
    else if(valid_anim_state(anim,dir)) {
        current_tile = &m_animations[anim][dir];
    }
    else {return AnimSignal::missing;}

    if(m_anim_state != anim || m_direction != dir) {
        // Set rendering dimensions to current tile
        m_transform.set_dimensions(current_tile->get_w(), current_tile->get_h());
        m_anim_state = anim;
        m_direction = dir;
        current_tile->init_anim();
    }

    return current_tile->push_anim_trigger(speed);
}

/// Checks if the currently set animation state and direction are existing
bool Actor::valid_anim_state(std::string anim, Direction dir) const {
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
 * @return @c Rect The hitbox
 * @note If there is no valid hitbox an empty one gets returned
 *
 * First the actor checks if the active animation has the hitbox with the name
 * and returns it instead.
 */
Rect Actor::get_hitbox(std::string type) const {


    Rect current_hitbox = {0,0,0,0};
    // Try extracting hitbox from currenty active animated tile
    if(m_anim_state != AnimationType::none && valid_anim_state()) {
        current_hitbox = m_animations.at(m_anim_state).at(m_direction).get_hitbox(type);
    }
    // If that failed, extract hitbox from base actor tile
    if(current_hitbox.empty()) {current_hitbox = m_base_tile.get_hitbox(type);}
    // If that also failed just return the empty hitbox
    if(current_hitbox.empty()) {return current_hitbox;}

    // Otherwise adjust hitbox position and return
    m_transform.transform_hitbox(current_hitbox);

    return current_hitbox;
}

/**
 * @brief Returns all active hitboxes
 *
 * To the hitboxes of the actor tile, possible hitboxes of the active
 * animation and its animation frame are added. Specific ones may override general ones.
 */
const std::map<std::string, Rect> Actor::get_hitboxes() const {
    // Get all hitboxes from base tile
    std::map<std::string, Rect> hitboxes = m_base_tile.get_hitboxes();
    // If there is a valid animation tile, load those "ontop" of the other hitboxes
    if(m_anim_state != AnimationType::none && valid_anim_state()) {
        for(const auto& hitbox_pair: m_animations.at(m_anim_state).at(m_direction).get_hitboxes()) {
            hitboxes[hitbox_pair.first] = hitbox_pair.second;
        }
    }
    // Adjust each hitbox position and return
    for(auto& hitbox_pair : hitboxes) {
        m_transform.transform_hitbox(hitbox_pair.second);
    }
    return hitboxes;
}

/**
 * @brief Checks if the actor is standing on ground
 * @param dir The direction of gravity
 * @return @c bool which is True if the actor is on ground
 */
bool Actor::on_ground(Collidees target, std::string my_hitbox, const std::vector<std::string>& other_hitboxes, Direction dir, int tolerance) const {
    Rect pos = get_hitbox(my_hitbox);
    if(pos.empty()) {return false;}
    Rect temp;
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
    return m_map->get_layer_collection().check_collision(temp, target,other_hitboxes);
}

bool Actor::separate(TileInstance& tile, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = tile.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
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
    if(&actor == this) {return false;}
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = actor.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
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

bool Actor::separate(const Rect& first, const Rect& second) {
    Rect intersect = first.get_intersection(second);
    if(intersect.empty()) {return false;}
    Point delta = rect_center_difference(first,second);

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
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = tile.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
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
    if(&actor == this) {return false;}
    bool moved = false;
    for(std::string first_hitbox_name : my_hitboxes) {
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = actor.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
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


bool Actor::separate_along_path(float x, float y,const Rect& first, const Rect& second) {
    if(!first.has_intersection(second)) {return false;}
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
                Point upper_left_corner = {first.x,first.y};
                float right_dist = second.x + second.w - upper_left_corner.x;
                float down_dist = second.y + second.h - upper_left_corner.y;

                float x_steps = right_dist / std::abs(x);
                float y_steps = down_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(right_dist,x_steps * y);
                }
                else {
                    move_relative(y_steps * x,down_dist);
                }
            }
            // Move Right Up
            else {
                Point lower_left_corner = {first.x,first.y + first.h};
                float right_dist = second.x + second.w - lower_left_corner.x;
                float up_dist = lower_left_corner.y - second.y;

                float x_steps = right_dist / std::abs(x);
                float y_steps = up_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(right_dist, x_steps * y);
                }
                else {
                    move_relative(y_steps * x,-up_dist);
                }
            }
        }
        else {
            // Move Left Down
            if(y > 0.0) {
                Point upper_right_corner = {first.x+first.w,first.y};
                float left_dist = upper_right_corner.x - second.x;
                float down_dist = second.y + second.h - upper_right_corner.y;

                float x_steps = left_dist / std::abs(x);
                float y_steps = down_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(-left_dist, x_steps * y);
                }
                else {
                    move_relative(y_steps * x,down_dist);
                }
            }
            // Move Left Up
            else {
                Point lower_right_corner = {first.x+first.w,first.y+first.h};
                float left_dist = lower_right_corner.x - second.x;
                float up_dist = lower_right_corner.y - second.y;

                float x_steps = left_dist / std::abs(x);
                float y_steps = up_dist / std::abs(y);

                if(x_steps < y_steps) {
                    move_relative(-left_dist, x_steps * y);
                }
                else {
                    move_relative(y_steps * x,-up_dist);
                }
            }
        }
    }
    return true;
}

bool Actor::separate_along_path(float x1, float y1, float x2, float y2, Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    if(&actor == this) {return false;}
    auto old_pos = m_transform.get_relative(0.0,1.0);
    float old_x = old_pos.x;
    float old_y = old_pos.x;

    // Combine both separation vectors
    float combined_x = x1 - x2;
    float combined_y = y1 - y2;

    // Separate this actor from other by combined vector
    if(separate_along_path(combined_x,combined_y,actor,my_hitboxes,other_hitboxes,false)) {
        // Get the actual separation vector
        auto new_pos = m_transform.get_relative(0.0,1.0);
        float delta_x = new_pos.x - old_x;
        float delta_y = new_pos.y - old_y;
        move_absolute(old_x,old_y);

        // Get factor from nonzero combined vector component
        float factor;
        if(combined_x != 0.0) {
            // Get factor which determines how "much" of the combined vector is in the final vector
            factor = delta_x / combined_x;
        }
        else if(combined_y != 0.0) {
            // Get factor which determines how "much" of the combined vector is in the final vector
            factor = delta_y / combined_y;
        }
        else {
            Logger(Logger::error) << "You cant separate two actors by equal separation vectors!";
            return false;
        }

        // Apply the two base vectors by determined factor to each actor
        move_relative(factor * x1, factor * y1);
        actor.move_relative(factor * x2, factor * y2);

        return true;
    }
    // If there wasn't an intersection at all, return false
    else {
        return false;
    }
}


bool Actor::check_collision(Actor& other, bool notify) {
    bool collided = false;
    for(auto first_hitbox : get_hitboxes()) {
        Rect& first = first_hitbox.second;
        if(first.empty()) {continue;}
        for(auto second_hitbox : other.get_hitboxes()) {
            Rect& second = second_hitbox.second;
            if(second.empty()) {continue;}
            if(first.has_intersection(second)) {
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
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = other.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
            if(first_hitbox.has_intersection(second_hitbox)) {
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
        Rect& first = first_hitbox.second;
        if(first.empty()) {continue;}
        for(auto second_hitbox : other.get_hitboxes()) {
            Rect& second = second_hitbox.second;
            if(second.empty()) {continue;}
            if(first.has_intersection(second)) {
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
        Rect first_hitbox = get_hitbox(first_hitbox_name);
        if(first_hitbox.empty()) {continue;}
        for(std::string second_hitbox_name : other_hitboxes) {
            Rect second_hitbox = other.get_hitbox(second_hitbox_name);
            if(second_hitbox.empty()) {continue;}
            if(first_hitbox.has_intersection(second_hitbox)) {
                collided = true;
                if(notify) {
                    add_collision({other,first_hitbox_name,second_hitbox_name});
                }
            }
        }
    }
    return collided;
}

}} // namespace salmon::internal

