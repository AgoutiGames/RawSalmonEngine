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

#include <SDL2/SDL.h>
#include <vector>
#include <map>

#include "util/tinyxml2.h"
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
    void render(SDL_Rect& dest) const; // Resizable render

    // Avoids daisy chaining of animated tiles
    const SDL_Rect& get_clip_self() const {return m_clip;}
    const SDL_Rect& get_hitbox() const {return m_hitbox;}

    tinyxml2::XMLError parse_tile(tinyxml2::XMLElement* source);
    tinyxml2::XMLError parse_actor_anim(tinyxml2::XMLElement* source);
    tinyxml2::XMLError parse_actor_templ(tinyxml2::XMLElement* source);

    void init_anim();
    void init_anim(Uint32 time);

    bool push_anim();
    AnimSignal push_anim_trigger();
    void push_anim(Uint32 time);

    std::string get_type() const {return m_type;}

    Tileset& get_tileset() {return *mp_tileset;}

private:
    const SDL_Rect& get_clip() const;

    Tileset* mp_tileset = nullptr;
    SDL_Rect m_clip;
    SDL_Rect m_hitbox = {0,0,0,0};
    Uint16 m_trigger_frame = 0;
    std::string m_type;
    bool m_animated = false;

    // Variables required for animated tiles
    Uint16 m_current_id = 0;
    std::vector<Uint16> m_anim_ids; // could use Tile* for better performance but greater memory allocation
    std::vector<unsigned> m_durations;
    Uint32 m_anim_timestamp = 0;
};



#endif // TILE_HPP_INCLUDED
