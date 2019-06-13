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

#include "actor/cause.hpp"
#include "actor/data_block.hpp"
#include "event/event_collection.hpp"
#include "event/event_queue.hpp"
#include "map/tile.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

class MapData;
class Actor;

struct ActorTemplate {
    std::string template_name = "_";
    Direction direction = Direction::down;
    std::map<std::string, SDL_Rect> hitbox;
    std::map<AnimationType, std::map<Direction, Tile>> animations;
    EventCollection<Actor, Response> response; ///< Map which yields events for response values
};

/**
 * @brief Parse, store and manage all actors
 *
 * @note Actors can only be constructed from parsed @c ActorTemplate
 */

class Actor{

    public:
        // Constructors
        // I have no idea why but with a default constructor
        // this won't compile properly
        // This probably has something to do with members lacking one
        Actor(Uint16 tile_id, MapData* map);          ///< Construct actor from tile_id corresponding to an ActorTemplate
        Actor(const ActorTemplate& templ, MapData* map);    ///< Construct actor from an ActorTemplate

        // Core functions
        tinyxml2::XMLError init_actor(tinyxml2::XMLElement* source);
        bool update();
        bool animate(AnimationType anim = AnimationType::current, Direction dir = Direction::current);
        AnimSignal animate_trigger(AnimationType anim = AnimationType::current, Direction dir = Direction::current);
        void render(int x_cam, int y_cam) const;
        bool move(float x_factor, float y_factor);
        bool unstuck();
        bool collide(const SDL_Rect* rect, int& x_depth, int& y_depth, std::string type = "COLLIDE") const;
        bool collide(const SDL_Rect* rect, std::string type = "COLLIDE") const;
        bool on_ground(Direction dir = Direction::down, int tolerance = 0) const;
        bool respond(Response r, Cause c = Cause());

        // Getters
        AnimationType get_animation() const {return m_anim_state;}
        Direction get_direction() const {return m_direction;}
        std::string get_name() const {return m_name;}
        std::string get_type() const {return m_type;}
        MapData& get_map() {return *m_map;}
        float get_x() const {return m_x;}
        float get_y() const {return m_y;}
        unsigned get_w() const {return m_width;}
        unsigned get_h() const {return m_height;}
        int get_x_center() const {return static_cast<int>(m_x + (m_width / 2));}
        int get_y_center() const {return static_cast<int>(m_y - (m_height / 2));}

        DataBlock& get_data() {return m_data;}

        SDL_Rect get_hitbox(std::string type = "COLLIDE") const;
        const std::map<std::string, SDL_Rect> get_hitboxes() const;

        EventQueue<Actor>& get_event_queue() {return m_events;}

    private:
        MapData* m_map;

        float m_x; ///< Coordinate of lower left image corner
        float m_y; ///< @warning LOWER left corner!
        unsigned m_width;
        unsigned m_height;
        std::string m_name;
        std::string m_type;

        AnimationType m_anim_state = AnimationType::idle; ///< Currently active animation
        Direction m_direction; ///< Current direction facing
        std::map<std::string, SDL_Rect> m_hitbox; ///< All hitboxes adressed by type
        std::map<AnimationType, std::map<Direction, Tile>> m_animations; ///< 2D Map which stores all animation tiles

        EventCollection<Actor, Response> m_response; ///< EventCollection which yields events for response values

        EventQueue<Actor> m_events;

        DataBlock m_data; ///< This holds custom user values by string
};

/// Overloading of comparision operator to enable the sort() functionality in a std::list
inline bool operator< (const Actor& lhs, const Actor& rhs) {
    if(lhs.get_y() != rhs.get_y()) {
        return lhs.get_y() < rhs.get_y();
    }
    else {
        return lhs.get_x() < rhs.get_x();
    }
}

#endif // ACTOR_HPP_INCLUDED
