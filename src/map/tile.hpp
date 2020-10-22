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
#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

#include <SDL.h>
#include <vector>
#include <map>
#include <tinyxml2.h>

#include "transform.hpp"
#include "util/game_types.hpp"

namespace salmon { namespace internal {

class Tileset; // forward declaration

/**
 * @brief Parse, store and manage an individual tile
 */

class Tile{
public:
    Tile() = default;
    Tile(Tileset* ts, const SDL_Rect& clp); // The initializing constructor

    void render(float x, float y) const;
    void render_extra(float x, float y, double angle, bool x_flip = false, bool y_flip = false, float x_center = 0.5, float y_center = 0.5) const;
    void render(Rect& dest) const; // Resizable render
    void render_extra(Rect& dest, double angle, bool x_flip = false, bool y_flip = false, float x_center = 0.5, float y_center = 0.5) const;

    Rect get_hitbox(std::string name = DEFAULT_HITBOX, bool aligned = false) const;
    std::map<std::string, Rect> get_hitboxes(bool aligned = false) const;

    tinyxml2::XMLError parse_tile(tinyxml2::XMLElement* source, bool skip_properties = false);
    tinyxml2::XMLError parse_actor_anim(tinyxml2::XMLElement* source);
    tinyxml2::XMLError parse_actor_templ(tinyxml2::XMLElement* source);

    void init_anim(Uint32 time = SDL_GetTicks());

    bool push_anim(float speed = 1.0f, Uint32 time = SDL_GetTicks());
    AnimSignal push_anim_trigger(float speed = 1.0f, Uint32 time = SDL_GetTicks());
    bool set_frame(int anim_frame, Uint32 time = SDL_GetTicks());
    int get_frame_count() const {return m_anim_ids.size();}
    int get_current_frame() const {return m_current_id;}
    bool is_valid() const {return mp_tileset != nullptr;}

    std::string get_type() const {return m_type;}
    Tileset& get_tileset() {return *mp_tileset;}

    int get_w() const {return get_clip().w;}
    int get_h() const {return get_clip().h;}

private:
    Rect get_hitbox_self(std::string name = DEFAULT_HITBOX, bool aligned = false) const;
    const std::map<std::string, Rect> get_hitboxes_self(bool aligned = false) const;

    const SDL_Rect& get_clip_self() const {return m_clip;}
    const SDL_Rect& get_clip() const;

    Tileset* mp_tileset = nullptr;
    SDL_Rect m_clip;
    std::map<std::string, Rect> m_hitboxes; // Origin at upper left corner of tile
    std::string m_type = "";
    bool m_animated = false;

    // Variables required for animated tiles
    unsigned m_current_id = 0;
    unsigned m_trigger_frame = 0;
    std::vector<Uint32> m_anim_ids; // could use Tile* for better performance but greater memory allocation
    std::vector<unsigned> m_durations;
    Uint32 m_anim_timestamp = 0;
    float m_time_delta = 0;
};

class TileInstance {
    public:
        TileInstance(Tile* tile, Transform t) : m_tile{tile}, m_transform{t} {}

        Rect get_hitbox(std::string name = DEFAULT_HITBOX, bool aligned = false) const {
            Rect temp = m_tile->get_hitbox(name,aligned);
            m_transform.transform_hitbox(temp);
            return temp;
        }
        std::map<std::string, Rect> get_hitboxes(bool aligned = false) const {
            std::map<std::string, Rect> hitboxes = m_tile->get_hitboxes(aligned);
            for(auto& hb : hitboxes) {m_transform.transform_hitbox(hb.second);;}
            return hitboxes;
        }
        Tile* get_tile() const {return m_tile;}
        const Transform& get_transform() const {return m_transform;}

        bool valid() const {return (m_tile == nullptr) ? false : true ;}

    private:
        Tile* m_tile = nullptr;
        Transform m_transform;
};
}} // namespace salmon::internal

#endif // TILE_HPP_INCLUDED
