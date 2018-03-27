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
#ifndef ACTOR_HPP_INCLUDED
#define ACTOR_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>

#include "event/actor_event.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

class MapData;

/**
 * @brief Parse, store and manage all actors
 *
 * @note Actors can only be constructed from parsed @c ActorTemplate
 */

class Actor{

    public:
        Actor(Uint16 tile_id, MapData* map);          ///< Construct actor from tile_id corresponding to an ActorTemplate
        Actor(const ActorTemplate& templ, MapData* map);    ///< Construct actor from an ActorTemplate

        tinyxml2::XMLError init_actor(tinyxml2::XMLElement* source);
        bool update();
        bool animate(AnimationType anim, Direction dir);
        void render(int x_cam, int y_cam) const;
        bool move(float x_factor, float y_factor);
        bool process_events();
        void add_event(ActorEvent* event);
        bool collide(const SDL_Rect* rect, int& x_depth, int& y_depth) const;
        bool respond(Response r, Actor* cause = nullptr, SDL_Keysym key = SDL_Keysym());

        AnimationType get_animation() const {return m_anim_state;}
        Direction get_direction() const {return m_direction;}
        std::string get_name() const {return m_name;}
        std::string get_type() const {return m_type;}
        float get_x() const {return m_x;}
        float get_y() const {return m_y;}
        unsigned get_w() const {return m_width;}
        unsigned get_h() const {return m_width;}
        int get_x_center() const {return static_cast<int>(m_x + (m_width / 2));}
        int get_y_center() const {return static_cast<int>(m_y - (m_height / 2));}
        void set_cooldown(std::string event_type, float dur_sec) {m_timestamp[event_type] = SDL_GetTicks() + dur_sec * 1000;}
        Uint32 get_cooldown(std::string event_type) const {return m_timestamp.at(event_type);}
        void block_event(std::string event_type) {m_block[event_type] = true;}
        void unblock_event(std::string event_type) {m_block[event_type] = false;}
        bool is_blocked(std::string event_type) const;
        bool in_cooldown(std::string event_type) const;



    private:
        MapData* m_map;

        float m_x; ///< Coordinate of lower left image corner
        float m_y; ///< @warning LOWER left corner!
        unsigned m_width;
        unsigned m_height;
        std::string m_name;
        std::string m_type;
        float m_base_speed;

        AnimationType m_anim_state = AnimationType::idle; ///< Currently active animation
        Direction m_direction; ///< Current direction facing
        SDL_Rect m_hitbox;
        std::map<AnimationType, std::map<Direction, Tile>> m_animations; ///< 2D Map which stores all animation tiles
        std::map<Response, ActorEvent*> m_response; ///< Map which yields events for response values

        std::map<std::string, Uint32> m_timestamp; ///< Map holding timestamps for use as cooldown functionality
        std::map<std::string, bool> m_block; ///< Map determinig if the event pipeline is blocked for a specific event type
        std::vector<ActorEvent*> m_event_pipeline; ///< Vector of current events to be processed
};

/// Overloading of comparision operator to enable the sort() functionality in a std::list
inline bool operator< (const Actor& lhs, const Actor& rhs) {return lhs.get_y() < rhs.get_y();}


#endif // ACTOR_HPP_INCLUDED
