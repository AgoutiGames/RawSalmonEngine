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

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <iostream>

#include "actor/actor.hpp"
#include "graphics/texture.hpp"
#include "map/mapdata.hpp"
#include "map/tileset.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

/**
 * @brief Construct an empty tile
 */
Tile::Tile() {

}

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
 * @param first_gid The first global tile id of the tileset
 * @param base_map Reference to map object to register possible animated status
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_tile(tinyxml2::XMLElement* source, unsigned first_gid, MapData& base_map) {
    using namespace tinyxml2;

    XMLError eResult;

    // Parse user specified properties of the tile (only speed right now)
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            else if(name == "SPEED") {
                eResult = p_property->QueryFloatAttribute("value", &m_speed);
                if(eResult != XML_SUCCESS) return eResult;
            }
            else {
                std::cerr << "Unknown tile property \""<< p_name << "\" specified\n";
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

            m_hitbox = temp_rec;
        }
    }

    // Parse the animation info
    XMLElement* p_animation = source->FirstChildElement("animation");
    if(p_animation != nullptr) {
        XMLElement* p_frame = p_animation->FirstChildElement("frame");

        m_animated = true;
        base_map.set_tile_animated(this);

        // Parse each animation frame
        while(p_frame != nullptr) {
            unsigned anim_tile_id;
            unsigned duration;
            eResult = p_frame->QueryUnsignedAttribute("tileid", &anim_tile_id);
            if(eResult != XML_SUCCESS) return eResult;
            eResult = p_frame->QueryUnsignedAttribute("duration", &duration);
            if(eResult != XML_SUCCESS) return eResult;

            // We don't use local, but global tile ids for animation
            anim_tile_id += first_gid;

            // The actual registration of the frame
            m_anim_ids.push_back(static_cast<Uint16>(anim_tile_id));
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
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_actor_anim(tinyxml2::XMLElement* source, unsigned first_gid, MapData& base_map) {
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
                    std::cerr << "Invalid actor name in actor animation\n";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }

            // Parse animation type
            else if(name == "ANIMATION_TYPE") {
                const char* p_anim_type = p_property->Attribute("value");
                if(p_anim_type != nullptr) {
                    anim = str_to_anim_type(std::string(p_anim_type));
                    if(anim == AnimationType::invalid) {
                        std::cerr << "Invalid animation type \"" << p_anim_type << "\" in actor animation for " << actor_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    std::cerr << "Missing animation type in actor animation for " << actor_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }
            }

            // Parse current direction
            else if(name == "DIRECTION") {
                const char* p_direction = p_property->Attribute("value");
                if(p_direction != nullptr) {
                    dir = str_to_direction(std::string(p_direction));
                    if(dir == Direction::invalid) {
                        std::cerr << "Invalid animation type \"" << p_direction << "\" in actor animation for " << actor_name << "\n";
                        return XML_WRONG_ATTRIBUTE_TYPE;
                    }
                }
                else {
                    std::cerr << "Missing direction in actor animation for " << actor_name << "\n";
                    return XML_NO_ATTRIBUTE;
                }

            }
            else {
                std::cerr << "Unknown tile property \""<< p_name << "\" specified\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            p_property = p_property->NextSiblingElement("property");
        }

    }

    else {
        std::cerr << "Missing properties on actor_animation tile\n";
        return XML_NO_ATTRIBUTE;
    }

    // Parse the animation info
    XMLElement* p_animation = source->FirstChildElement("animation");
    if(p_animation != nullptr) {
        XMLElement* p_frame = p_animation->FirstChildElement("frame");

        m_animated = true;
        // Animation of actor tiles are externally handled

        // Parse each animation frame
        while(p_frame != nullptr) {
            unsigned anim_tile_id;
            unsigned duration;
            eResult = p_frame->QueryUnsignedAttribute("tileid", &anim_tile_id);
            if(eResult != XML_SUCCESS) return eResult;
            eResult = p_frame->QueryUnsignedAttribute("duration", &duration);
            if(eResult != XML_SUCCESS) return eResult;

            // We don't use local, but global tile ids for animation
            anim_tile_id += first_gid;

            // The actual registration of the frame
            m_anim_ids.push_back(static_cast<Uint16>(anim_tile_id));
            m_durations.push_back(duration);

            // Go to next frame
            p_frame = p_frame->NextSiblingElement("frame");
        }
    }
    else {
        std::cerr << "Missing tile animation on actor animation for " << actor_name << "\n";
        return XML_NO_ATTRIBUTE;
    }

    if(actor_name == "_") {
        std::cerr << "Missing actor name in actor animation \n";
        return XML_NO_ATTRIBUTE;
    }

    else if(anim == AnimationType::invalid) {
        std::cerr << "Missing animation type in actor animation for " << actor_name << "\n";
        return XML_NO_ATTRIBUTE;
    }

    else if(dir == Direction::invalid) {
        std::cerr << "Missing direction in actor animation for " << actor_name << "\n";
        return XML_NO_ATTRIBUTE;
    }

    else {
        // Add this animated tile to the actor template
        base_map.add_actor_animation(actor_name, anim, dir, this);
    }

    return XML_SUCCESS;
}

/**
 * @brief Parse tile information of actor template tiles
 * @param source The @c XMLElement from the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Parse the tile via the static Actor method @c add_template
 */
tinyxml2::XMLError Tile::parse_actor_templ(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;
    XMLError eResult;

    eResult = base_map.add_actor_template(source, this);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed at adding actor template\n";
        return XML_ERROR_PARSING_ELEMENT;
    }
    return XML_SUCCESS;
}

Tile::~Tile() {

}

const SDL_Rect& Tile::get_clip(const MapData& base_map) const {
    if(m_animated) {
        // Avoids daisy chaining of animated tiles
        return base_map.get_tile(m_anim_ids[m_current_id])->get_clip_self();
    }
    else {
        return m_clip;
    }
}

/// Initialize the tile to the current timestamp and first frame
void Tile::init_anim() {
    m_current_id = 0;
    m_anim_timestamp = SDL_GetTicks();
}

/// Initialize the tile to the supplied timestamp and first frame
void Tile::init_anim(Uint32 time) {
    m_current_id = 0;
    m_anim_timestamp = time;
}

/**
 * @brief Animates a tile
 * @return a @c bool which indicates if the animation reached it's starting point/ frame 0
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 */
bool Tile::push_anim() {
    bool wrap_around = false;
    Uint32 time = SDL_GetTicks();
    if(time - m_anim_timestamp >= m_durations[m_current_id]) {
        m_current_id++;
        m_anim_timestamp = time;
        if(m_current_id >= m_anim_ids.size()) {
            m_current_id = 0;
            wrap_around = true;
        }
    }
    return wrap_around;
}

/**
 * @brief Animates a tile with supplied timestamp
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 */
void Tile::push_anim(Uint32 time) {
    if(time - m_anim_timestamp >= m_durations[m_current_id]) {
        m_current_id++;
        m_anim_timestamp = time;
        if(m_current_id >= m_anim_ids.size()) {
            m_current_id = 0;
        }
    }
}

/**
 * @brief Render a tile object to a coordinate
 * @param x, y The specified coordinates
 */
void Tile::render(int x, int y, const MapData& base_map) const {
    x += mp_tileset->get_x_offset();
    y += mp_tileset->get_y_offset() - (mp_tileset->get_tile_height() - base_map.get_tile_h());
    const Texture* image = mp_tileset->get_image_pointer();

    image->render(base_map.get_renderer(), x, y, &get_clip(base_map));
    return;
}

/**
 * @brief Render a tile object to a rect
 * @param dest The rendering rect
 *
 * This function can resize the tile image
 */
void Tile::render(SDL_Rect& dest, const MapData& base_map) const {
    dest.x += mp_tileset->get_x_offset();
    dest.y += mp_tileset->get_y_offset();
    const Texture* image = mp_tileset->get_image_pointer();

    image->render_resize(base_map.get_renderer(), &get_clip(base_map), &dest);
    return;
}

