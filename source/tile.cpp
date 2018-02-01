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
#include <vector>
#include <string>
#include <iostream>
#include "SDL2/SDL.h"
#include "tinyxml2.h"
#include "tile.hpp"
#include "texture.hpp"
#include "tileset.hpp"
#include "game_types.hpp"
#include "actor.hpp"

std::vector<Tile*> Tile::mp_tiles;
std::vector<Uint16> Tile::m_anim_tiles;
SDL_Renderer** Tile::mpp_renderer = nullptr;
unsigned Tile::m_base_tile_w = 0;
unsigned Tile::m_base_tile_h = 0;
Tile Tile::empty_tile;

/// Initialize the whole Tile class
void Tile::initialize(SDL_Renderer** renderer, unsigned base_tile_w, unsigned base_tile_h) {
    mp_tiles.clear();
    m_anim_tiles.clear();
    empty_tile = Tile(true);
    mpp_renderer = renderer;
    m_base_tile_w = base_tile_w;
    m_base_tile_h = base_tile_h;
}

/**
 * @brief Construct an empty tile
 * @param append Determines if the Tile should be appended to @c mp_tiles
 */
Tile::Tile(bool append) {
    if(append == true) {
        mp_tiles.push_back(this);
        m_gid = mp_tiles.size() - 1;
    }
    else {
        m_gid = 0;
    }
}

/**
 * @brief Construct and registers a fully functional tile
 * @param ts Pointer to the corresponding tileset
 * @param clp @c SDL_Rect which determines the snippet of the whole tileset image
 */
Tile::Tile(Tileset* ts, SDL_Rect& clp) :
mp_tileset{ts}, m_clip{clp}
{
    mp_tiles.push_back(this);
    m_gid = mp_tiles.size() - 1;
}

/*Tile::Tile(const Tile& other) :
m_gid{other.m_gid}, mp_tileset{other.mp_tileset}, m_clip{other.m_clip},
m_hitbox{other.m_hitbox}, m_speed{other.m_speed}, m_animated{other.m_animated},
m_current_id{other.m_current_id}, m_anim_ids{other.m_anim_ids},
m_durations{other.m_durations}, m_anim_timestamp{other.m_anim_timestamp}

{
        mp_tiles[m_gid] = this;
}*/

/// See @c operator=
Tile::Tile(const Tile& other) {
    *this = other;
}

/**
 * @brief Assigns the values of a tile to another tile
 *
 * The most recent tile is always the active one referred in @c mp_tiles
 * this means, if several copies of a tile exist on screen, only the
 * newest one will be animated
 */
Tile& Tile::operator=(const Tile& other) {
    if(this != &other) {
        m_gid = other.m_gid;
        mp_tileset = other.mp_tileset;
        m_clip = other.m_clip;
        m_hitbox = other.m_hitbox;
        m_speed = other.m_speed;
        m_animated = other.m_animated;
        m_current_id = other.m_current_id;
        m_anim_ids = other.m_anim_ids;
        m_durations = other.m_durations;
        m_anim_timestamp = other.m_anim_timestamp;

        mp_tiles[m_gid] = this;
    }

    return *this;
}

/**
 * @brief Parse tile information from tileset
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_from_tileset(tinyxml2::XMLElement* source, unsigned first_gid) {
    using namespace tinyxml2;

    XMLError eResult;

    while(source != nullptr) {
        unsigned tile_id;
        eResult = source->QueryUnsignedAttribute("id", &tile_id);
        if(eResult != XML_SUCCESS) return eResult;
        tile_id += first_gid;

        Tile* T = mp_tiles[tile_id];
        const char* p_type;
        p_type = source->Attribute("type");

        std::string tile_type;
        if(p_type != nullptr) tile_type = p_type;

        if(p_type == nullptr) {
            eResult = T->parse_tile(source, first_gid);
        }
        else if(tile_type == "ACTOR_ANIMATION") {
            eResult = T->parse_actor_anim(source, first_gid);
        }
        else if(tile_type == "ACTOR_TEMPLATE") {
            eResult = T->parse_actor_templ(source);
        }
        else {
            std::cerr << "Unknown tile type: " << tile_type << "\n";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }

        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading tile: " << tile_id << "\n";
            return eResult;
        }

        source = source->NextSiblingElement();
    }
    return XML_SUCCESS;
}

/**
 * @brief Parse tile information of standard tiles
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tile::parse_tile(tinyxml2::XMLElement* source, unsigned first_gid) {
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
        m_anim_tiles.push_back(m_gid);

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
tinyxml2::XMLError Tile::parse_actor_anim(tinyxml2::XMLElement* source, unsigned first_gid) {
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
        // m_anim_tiles.push_back(m_gid); // Animation of actor tiles are

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
        Actor::add_animation(actor_name, anim, dir, this);
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
tinyxml2::XMLError Tile::parse_actor_templ(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;
    XMLError eResult;

    eResult = Actor::add_template(source, m_gid);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed at adding actor template\n";
        return XML_ERROR_PARSING_ELEMENT;
    }
    return XML_SUCCESS;
}

Tile::~Tile() {

}

const SDL_Rect& Tile::get_clip() const {
    if(m_animated) {
        return mp_tiles[m_anim_ids[m_current_id]]->m_clip;
    }
    else {
        return m_clip;
    }
}

/// Initializes all registered animated tiles to the current timestamp and first frame
void Tile::init_anim_tiles() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        mp_tiles[tile]->m_current_id = 0;
        mp_tiles[tile]->m_anim_timestamp = time;
    }
}

/// Initialize the tile to the current timestamp and first frame
void Tile::init_anim() {
    m_current_id = 0;
    m_anim_timestamp = SDL_GetTicks();
}

/**
 * @brief Animates all tiles
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 */
void Tile::push_all_anim() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        Tile* t = mp_tiles[tile];
        if(time - t->m_anim_timestamp >= t->m_durations[t->m_current_id]) {
            t->m_current_id++;
            t->m_anim_timestamp = time;

            if(t->m_current_id >= t->m_anim_ids.size()) t->m_current_id = 0;
        }
    }
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

/// Returns the pointer to a tile from it's tile id
Tile* Tile::get_tile(Uint16 tile_id) {
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        return nullptr;
    }
    else return mp_tiles[tile_id];
}

/**
 * @brief Renders a tile by it's id at a coordinate
 * @param x, y The rendering coordinates
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid
 */
bool Tile::render(Uint16 tile_id, int x, int y) {
    bool success = true;

    // Check if id is valid
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(x,y);
    }
    return success;
}

/**
 * @brief Renders a tile by it's id to a rect
 * @param dest The rendering rectangle
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid.
 * This function can resize the tile image
 */
bool Tile::render(Uint16 tile_id, SDL_Rect& dest) {
    bool success = true;
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(dest);
    }
    return success;
}

/**
 * @brief Render a tile object to a coordinate
 * @param x, y The specified coordinates
 */
void Tile::render(int x, int y) const {
    x += mp_tileset->get_x_offset();
    y += mp_tileset->get_y_offset() - (mp_tileset->get_tile_height() - m_base_tile_h);
    const Texture* image = mp_tileset->get_image_pointer();

    image->render(*mpp_renderer, x, y, &get_clip());
    return;
}

/**
 * @brief Render a tile object to a rect
 * @param dest The rendering rect
 *
 * This function can resize the tile image
 */
void Tile::render(SDL_Rect& dest) const {
    dest.x += mp_tileset->get_x_offset();
    dest.y += mp_tileset->get_y_offset();
    const Texture* image = mp_tileset->get_image_pointer();

    image->render_resize(*mpp_renderer, &get_clip(), &dest);
    return;
}

SDL_Renderer* Tile::get_renderer(){
    return *mpp_renderer;
}

/// Check if the position of the tiles in mp_tiles correspond to their gid
void Tile::diagnose() {
    for(unsigned i = 0; i < mp_tiles.size(); i++) {
        std::cerr << "mp_tiles " << i << ":  gid: " << mp_tiles[i]->m_gid << "\n";
    }
}


