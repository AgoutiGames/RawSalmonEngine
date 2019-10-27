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
#include "map/tile.hpp"

#include <string>
#include <iostream>

#include "actor/actor.hpp"
#include "graphics/texture.hpp"
#include "map/mapdata.hpp"
#include "map/tileset.hpp"
#include "util/logger.hpp"
#include "util/parse.hpp"

/**
 * @brief Construct and registers a fully functional tile
 * @param ts Pointer to the corresponding tileset
 * @param clp @c SDL_Rect which determines the snippet of the whole tileset image
 */
Tile::Tile(Tileset* ts, SDL_Rect& clp) :
mp_tileset{ts}, m_clip{clp}
{

}

/**
 * @brief Parse tile information of standard tiles
 * @param source The @c XMLElement from the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_tile(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    XMLError eResult;

    // Parse user specified properties of the tile (only speed right now)
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            const char* p_value;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            else if(name == "TYPE") {
                p_value = source->Attribute("value");
                if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                m_type = std::string(p_value);
            }

            else {
                Logger(Logger::error) << "Unknown tile property \""<< p_name << "\" specified";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            p_property = p_property->NextSiblingElement("property");
        }
    }

    // Parse the hitbox of the individual tile
    XMLElement* p_objgroup = source->FirstChildElement("objectgroup");
    if(p_objgroup != nullptr) {
        XMLElement* p_object = p_objgroup->FirstChildElement("object");
        if(p_object != nullptr) {
            eResult = parse::hitboxes(p_object, m_hitboxes);
            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Failed at parsing hitbox for tile";
                return eResult;
            }
        }
    }

    // Parse the animation info
    XMLElement* p_animation = source->FirstChildElement("animation");
    if(p_animation != nullptr) {
        XMLElement* p_frame = p_animation->FirstChildElement("frame");

        m_animated = true;
        mp_tileset->get_ts_collection().set_tile_animated(this);

        // Parse each animation frame
        while(p_frame != nullptr) {
            unsigned anim_tile_id;
            unsigned duration;
            eResult = p_frame->QueryUnsignedAttribute("tileid", &anim_tile_id);
            if(eResult != XML_SUCCESS) return eResult;
            eResult = p_frame->QueryUnsignedAttribute("duration", &duration);
            if(eResult != XML_SUCCESS) return eResult;

            // We don't use local, but global tile ids for animation
            anim_tile_id += mp_tileset->get_first_gid();

            // The actual registration of the frame
            m_anim_ids.push_back(static_cast<Uint32>(anim_tile_id));
            m_durations.push_back(duration);

            // Go to next frame
            p_frame = p_frame->NextSiblingElement("frame");
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Parse tile information of actor animation tiles
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @param base_map Reference to map object to register as actor animation
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_actor_anim(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    // Initialize temporary variables
    std::string actor_name = "_";
    AnimationType anim = AnimationType::invalid;
    Direction dir = Direction::invalid;


    // Parse user specified properties of the tile
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);

            // Parse actor name
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

            else if(name == "ACTOR_NAME") {
                const char* p_actor_name = p_property->Attribute("value");
                if(p_actor_name != nullptr) actor_name = p_actor_name;
                else {
                    Logger(Logger::error) << "Invalid actor name in actor animation";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }

            // Parse animation type
            else if(name == "ANIMATION_TYPE") {
                const char* p_anim_type = p_property->Attribute("value");
                if(p_anim_type != nullptr) {
                    anim = str_to_anim_type(std::string(p_anim_type));
                    if(anim == AnimationType::invalid) {
                        Logger(Logger::error) << "Invalid animation type \"" << p_anim_type << "\" in actor animation for " << actor_name;
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                    if(anim == AnimationType::current) {
                        Logger(Logger::error) << "You can't define a specific animation type as the current one";
                        Logger(Logger::error) << "Invalid animation type \"" << p_anim_type << "\" in actor animation for " << actor_name;
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                    if(anim == AnimationType::none) {
                        Logger(Logger::error) << "You can't define a specific animation type as no animation";
                        Logger(Logger::error) << "Invalid animation type \"" << p_anim_type << "\" in actor animation for " << actor_name;
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    Logger(Logger::error) << "Missing animation type in actor animation for " << actor_name;
                    return XML_NO_ATTRIBUTE;
                }
            }

            // Parse current direction
            else if(name == "DIRECTION") {
                const char* p_direction = p_property->Attribute("value");
                if(p_direction != nullptr) {
                    dir = str_to_direction(std::string(p_direction));
                    if(dir == Direction::invalid) {
                        Logger(Logger::error) << "Invalid animation direction \"" << p_direction << "\" in actor animation for " << actor_name;
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                    if(dir == Direction::current) {
                        Logger(Logger::error) << "You can't define a specific direction as the current one";
                        Logger(Logger::error) << "Invalid animation direction \"" << p_direction << "\" in actor animation for " << actor_name;
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    Logger(Logger::error) << "Missing direction in actor animation for " << actor_name;
                    return XML_NO_ATTRIBUTE;
                }

            }

            else if(name == "TRIGGER_FRAME") {
                int frame;
                eResult = p_property->QueryIntAttribute("value", &frame);
                if(eResult != XML_SUCCESS) return eResult;
                if(frame < 0) {
                    Logger(Logger::error) << "Trigger frame can't be a negative value!";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
                m_trigger_frame = frame;
            }

            else {
                Logger(Logger::error) << "Unknown tile property \""<< p_name << "\" specified";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            p_property = p_property->NextSiblingElement("property");
        }

    }

    else {
        Logger(Logger::error) << "Missing properties on actor_animation tile";
        return XML_NO_ATTRIBUTE;
    }

    // Parse the animation info
    XMLElement* p_animation = source->FirstChildElement("animation");
    if(p_animation != nullptr) {
        XMLElement* p_frame = p_animation->FirstChildElement("frame");

        m_animated = true;

        // Parse each animation frame
        while(p_frame != nullptr) {
            unsigned anim_tile_id;
            unsigned duration;
            eResult = p_frame->QueryUnsignedAttribute("tileid", &anim_tile_id);
            if(eResult != XML_SUCCESS) return eResult;
            eResult = p_frame->QueryUnsignedAttribute("duration", &duration);
            if(eResult != XML_SUCCESS) return eResult;

            // We don't use local, but global tile ids for animation
            anim_tile_id += mp_tileset->get_first_gid();

            // The actual registration of the frame
            m_anim_ids.push_back(static_cast<Uint32>(anim_tile_id));
            m_durations.push_back(duration);

            // Go to next frame
            p_frame = p_frame->NextSiblingElement("frame");
        }
    }
    else {
        Logger(Logger::warning) << "Missing tile animation on actor animation for " << actor_name << " -> will use static tile instead";
        // return XML_NO_ATTRIBUTE;
    }

    if(actor_name == "_") {
        Logger(Logger::error) << "Missing actor name in actor animation";
        return XML_NO_ATTRIBUTE;
    }

    else if(anim == AnimationType::invalid) {
        Logger(Logger::error) << "Missing animation type in actor animation for " << actor_name;
        return XML_NO_ATTRIBUTE;
    }

    else if(dir == Direction::invalid) {
        Logger(Logger::error) << "Missing direction in actor animation for " << actor_name;
        return XML_NO_ATTRIBUTE;
    }

    else if(m_animated && m_trigger_frame >= m_anim_ids.size()) {
        Logger(Logger::error) << "The trigger frame " << m_trigger_frame << " is out of the animation range from 0 to " << m_anim_ids.size() - 1;
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    else {
        // Parse the hitboxes of the actor animation
        XMLElement* p_objgroup = source->FirstChildElement("objectgroup");
        if(p_objgroup != nullptr) {
            XMLElement* p_object = p_objgroup->FirstChildElement("object");
            if(p_object != nullptr) {
                eResult = parse::hitboxes(p_object, m_hitboxes);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Failed at parsing hitboxes of actor animation for actor " << actor_name;
                    return eResult;
                }
            }
        }
    }

    // Add this animated tile to the actor template
    MapData& base_map = mp_tileset->get_ts_collection().get_mapdata();
    base_map.add_actor_animation(actor_name, anim, dir, this);

    return XML_SUCCESS;
}

/**
 * @brief Parse tile information of actor template tiles
 * @param source The @c XMLElement from the tileset
 * @param base_map Reference to map object to register as actor template
 * @return an @c XMLError object which indicates success or error type
 *
 * Parse the tile via the static Actor method @c add_template
 */
tinyxml2::XMLError Tile::parse_actor_templ(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    eResult = mp_tileset->get_ts_collection().get_mapdata().add_actor_template(source, this);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed at adding actor template";
        return XML_ERROR_PARSING_ELEMENT;
    }
    return XML_SUCCESS;
}

/**
 * @brief Returns area of tileset image corresponding to tile
 * @param tsc Required for looking up other tiles
 *
 * For animated tiles the clip is the one of the current animation tile id,
 * instead of recursively calling this function again, get_clip_self is used which
 * always yields the normal clip value of the tile.
 *
 * If not animated the normal clip value is returned
 */
const SDL_Rect& Tile::get_clip() const {
    const TilesetCollection& tsc = mp_tileset->get_ts_collection();
    if(m_animated) {
        // Avoids daisy chaining of animated tiles
        return tsc.get_tile(m_anim_ids[m_current_id])->get_clip_self();
    }
    else {
        return m_clip;
    }
}

/// Initialize the tile to the supplied timestamp and first frame
void Tile::init_anim(Uint32 time) {
    m_current_id = 0;
    m_anim_timestamp = time;
}

/// Set tile to specific animation frame
bool Tile::set_frame(int anim_frame, Uint32 time) {
    if(anim_frame < 0 || static_cast<size_t>(anim_frame) >= m_anim_ids.size()) {
        return false;
    }
    m_current_id = static_cast<unsigned>(anim_frame);
    m_anim_timestamp = time;
    return true;
}

/**
 * @brief Animates a tile
 * @return a @c bool which indicates if the animation reached it's starting point/ frame 0
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 */
bool Tile::push_anim(float speed, Uint32 time) {
    if(!m_animated) {return true;}
    AnimSignal sig = push_anim_trigger(speed, time);
    if(sig == AnimSignal::wrap) {
        return true;
    }
    return false;
}

/**
 * @brief Animates a tile
 * @return a @c AnimSignal which indicates if the animation reached it's starting point/ frame 0
 *         or if it reached its trigger frame
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 * @note The wrap around signal has precedence over the trigger signal
 */
AnimSignal Tile::push_anim_trigger(float speed, Uint32 time) {
    if(!m_animated) {return AnimSignal::wrap;}
    // if(speed < 0.0f) {speed = 0.0f;}
    m_time_delta += speed * (time - m_anim_timestamp);
    m_anim_timestamp = time;
    AnimSignal sig = AnimSignal::none;

    // Backwards animation
    if(m_time_delta < 0) {

        unsigned id_before = m_current_id - 1;
        if(m_current_id == 0) {id_before = m_anim_ids.size() - 1;}

        while(-m_time_delta >= m_durations[id_before]) {
            m_time_delta += m_durations[id_before];

            if(m_current_id == 0) {
                m_current_id = m_anim_ids.size() - 1;
                if(sig < AnimSignal::wrap) {sig = AnimSignal::wrap;}

                id_before = m_current_id - 1;
            }
            else {
                m_current_id--;

                id_before = m_current_id - 1;
                if(m_current_id == 0) {id_before = m_anim_ids.size() - 1;}
            }

            if(m_current_id == m_trigger_frame) {
                if(sig < AnimSignal::trigger) {sig = AnimSignal::trigger;}
            }
            if(sig < AnimSignal::next) {sig = AnimSignal::next;}
        }
    }

    // Forward animation
    while(m_time_delta >= m_durations[m_current_id]) {
        m_time_delta -= m_durations[m_current_id];
        m_current_id++;
        if(m_current_id >= m_anim_ids.size()) {
            m_current_id = 0;
            if(sig < AnimSignal::wrap) {sig = AnimSignal::wrap;}
        }
        if(m_current_id == m_trigger_frame) {
            if(sig < AnimSignal::trigger) {sig = AnimSignal::trigger;}
        }
        if(sig < AnimSignal::next) {sig = AnimSignal::next;}
    }
    return sig;
}

/**
 * @brief Render a tile object to a coordinate
 * @param x, y The specified coordinates
 * @param tsc Reference to map for getting clip and renderer
 *
 * @note The coordinates are the upper left origin of the BASE tile
 *       this means that smaller or bigger tiles get shifted up or down
 *       to be aligned to the bottom left corner of the BASE tile
 */
void Tile::render(int x, int y) const {
    const TilesetCollection& tsc = mp_tileset->get_ts_collection();
    x += mp_tileset->get_x_offset();
    y += mp_tileset->get_y_offset() - (mp_tileset->get_tile_height() - tsc.get_tile_h());
    const Texture* image = mp_tileset->get_image_pointer();

    image->render(x, y, &get_clip());
    return;
}

/// @todo Add documentation
void Tile::render_extra(int x, int y, double angle, bool x_flip, bool y_flip) const {
    const TilesetCollection& tsc = mp_tileset->get_ts_collection();
    x += mp_tileset->get_x_offset();
    y += mp_tileset->get_y_offset() - (mp_tileset->get_tile_height() - tsc.get_tile_h());
    const Texture* image = mp_tileset->get_image_pointer();

    image->render_extra(x, y, &get_clip(), angle, x_flip, y_flip);
    return;
}

/**
 * @brief Render a tile object to a rect
 * @param dest The rendering rect
 * @param tsc Reference to map for getting clip and renderer
 *
 * @note This function can resize the tile image
 */
void Tile::render(SDL_Rect& dest) const {
    dest.x += mp_tileset->get_x_offset();
    dest.y += mp_tileset->get_y_offset();
    const Texture* image = mp_tileset->get_image_pointer();

    image->render_resize(&get_clip(), &dest);
    return;
}

/**
 * @brief Render a tile object to a rect
 * @param dest The rendering rect
 * @param tsc Reference to map for getting clip and renderer
 *
 * @note This function can resize the tile image
 */
void Tile::render_extra(SDL_Rect& dest, double angle, bool x_flip, bool y_flip) const {
    dest.x += mp_tileset->get_x_offset();
    dest.y += mp_tileset->get_y_offset();
    const Texture* image = mp_tileset->get_image_pointer();

    image->render_extra_resize(&get_clip(), &dest, angle, x_flip, y_flip);
    return;
}

/**
 * @brief Return the active hitbox by name
 * @param name The name/type of the hitbox
 * @param aligned Sets the origin of hitbox relative to tile grid
 *
 * The active hitbox is usually the hitbox stored within the m_hitbox member variable
 * but if the tile is animated it first checks if the currently active frame has the
 * hitbox of the given name and returns it instead
 */
SDL_Rect Tile::get_hitbox(std::string name, bool aligned) const {
    if(m_animated) {
        const TilesetCollection& tsc = mp_tileset->get_ts_collection();
        // Animation frame which is an animation itself doesn't make sense!
        // Explicitly request own hitbox
        SDL_Rect hitbox = tsc.get_tile(m_anim_ids[m_current_id])->get_hitbox_self(name, aligned);
        if(!SDL_RectEmpty(&hitbox)) {
            return hitbox;
        }
    }
    return get_hitbox_self(name, aligned);
}

/**
 * @brief Return the hitbox of this tile by name
 * @param name The name/type of the hitbox
 * @param aligned Sets the origin of hitbox relative to tile grid
 */
SDL_Rect Tile::get_hitbox_self(std::string name, bool aligned) const {
    if(m_hitboxes.find(name) == m_hitboxes.end()) {
        // std::cerr << "Could not find hitbox " << type << " for actor " << m_name << "\n";
        return SDL_Rect{0,0,0,0};
    }
    else{
        if(aligned) {
            SDL_Rect hitbox = m_hitboxes.at(name);
            const TilesetCollection& tsc = mp_tileset->get_ts_collection();
            hitbox.x += mp_tileset->get_x_offset();
            hitbox.y += mp_tileset->get_y_offset() - (mp_tileset->get_tile_height() - tsc.get_tile_h());
            return hitbox;
        }
        else {
            return m_hitboxes.at(name);
        }
    }
}

/**
 * @brief Return the active hitboxes
 * @param aligned Sets the origin of hitboxes relative to tile grid
 *
 * The active hitboxes are usually stored within the m_hitbox member variable
 * but if the tile is animated the hitboxes of the active frame get added and
 * may override the hitboxes of the base tile
 */
const std::map<std::string, SDL_Rect> Tile::get_hitboxes(bool aligned) const {
    if(m_animated) {
        std::map<std::string, SDL_Rect> hitboxes = get_hitboxes_self(aligned);

        const TilesetCollection& tsc = mp_tileset->get_ts_collection();
        // Animation frame which is an animation itself doesn't make sense!
        // Explicitly request own hitbox
        for(const auto& hitbox_pair: tsc.get_tile(m_anim_ids[m_current_id])->get_hitboxes_self(aligned)) {
            hitboxes[hitbox_pair.first] = hitbox_pair.second;
        }
        return hitboxes;
    }
    return get_hitboxes_self(aligned);
}

/**
 * @brief Return the hitboxes of this tile
 * @param aligned Sets the origin of hitboxes relative to tile grid
 */
const std::map<std::string, SDL_Rect> Tile::get_hitboxes_self(bool aligned) const {
    if(aligned) {
        std::map<std::string, SDL_Rect> hitboxes;
        for(auto& hb : m_hitboxes) {
            hitboxes[hb.first] = get_hitbox_self(hb.first, true);
        }
        return hitboxes;
    }
    else {
        return m_hitboxes;
    }
}

