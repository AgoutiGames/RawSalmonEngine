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
#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

#include <SDL.h>
#include <vector>
#include <map>
#include <tinyxml2.h>

#include "util/game_types.hpp"

class Tileset; // forward declaration

/**
 * @brief Parse, store and manage an individual tile
 */

class Tile{
public:
    Tile() = default;
    Tile(Tileset* ts, SDL_Rect& clp); // The initializing constructor

    void render(int x, int y) const;
    void render_extra(int x, int y, double angle, bool x_flip, bool y_flip) const;
    void render(SDL_Rect& dest) const; // Resizable render

    SDL_Rect get_hitbox(std::string name = "COLLIDE", bool aligned = false) const;
    const std::map<std::string, SDL_Rect> get_hitboxes(bool aligned = false) const;

    tinyxml2::XMLError parse_tile(tinyxml2::XMLElement* source);
    tinyxml2::XMLError parse_actor_anim(tinyxml2::XMLElement* source);
    tinyxml2::XMLError parse_actor_templ(tinyxml2::XMLElement* source);

    void init_anim(Uint32 time = SDL_GetTicks());

    bool push_anim(float speed = 1.0f, Uint32 time = SDL_GetTicks());
    AnimSignal push_anim_trigger(float speed = 1.0f, Uint32 time = SDL_GetTicks());

    std::string get_type() const {return m_type;}
    Tileset& get_tileset() {return *mp_tileset;}

private:
    SDL_Rect get_hitbox_self(std::string name = "COLLIDE", bool aligned = false) const;
    const std::map<std::string, SDL_Rect> get_hitboxes_self(bool aligned = false) const;

    const SDL_Rect& get_clip_self() const {return m_clip;}
    const SDL_Rect& get_clip() const;

    Tileset* mp_tileset = nullptr;
    SDL_Rect m_clip;
    std::map<std::string, SDL_Rect> m_hitboxes;
    std::string m_type = "";
    bool m_animated = false;

    // Variables required for animated tiles
    Uint32 m_current_id = 0;
    Uint32 m_trigger_frame = 0;
    std::vector<Uint32> m_anim_ids; // could use Tile* for better performance but greater memory allocation
    std::vector<unsigned> m_durations;
    Uint32 m_anim_timestamp = 0;
    float m_time_delta = 0;
};



#endif // TILE_HPP_INCLUDED
