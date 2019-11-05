/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#include <tinyxml2.h>

#include "actor/collision.hpp"
#include "actor/data_block.hpp"
#include "event/event_collection.hpp"
#include "event/event_queue.hpp"
#include "map/tile.hpp"
#include "util/game_types.hpp"

class MapData;

/**
 * @brief Parse, store and manage all actors
 *
 */
class Actor{

    public:
        // Actor() = default;
        Actor(MapData* map);

        // Core functions
        tinyxml2::XMLError parse_base(tinyxml2::XMLElement* source);
        tinyxml2::XMLError parse_properties(tinyxml2::XMLElement* source);
        void update();
        bool animate(AnimationType anim = AnimationType::current, Direction dir = Direction::current, float speed = 1.0);
        bool set_animation(AnimationType anim = AnimationType::current, Direction dir = Direction::current, int frame = 0);
        AnimSignal animate_trigger(AnimationType anim = AnimationType::current, Direction dir = Direction::current, float speed = 1.0);
        void render(int x_cam, int y_cam) const;
        bool move(float x_factor, float y_factor, bool absolute = false);
        bool unstuck();
        bool collide(const SDL_Rect* rect, int& x_depth, int& y_depth, std::string type = "COLLIDE") const;
        bool collide(const SDL_Rect* rect, std::string type = "COLLIDE") const;
        bool on_ground(Direction dir = Direction::down, int tolerance = 0) const;
        bool respond(Response r, Collision c = Collision());

        void set_w(unsigned w) {m_width = w;}
        void set_h(unsigned h) {m_height = h;}
        void set_name(std::string name) {m_name = name;}

        // Trivial Getters
        AnimationType get_animation() const {return m_anim_state;}
        Tile& get_animation_tile() {return m_animations.at(m_anim_state).at(m_direction);}
        std::map<AnimationType, std::map<Direction, Tile>>& get_animation_container() {return m_animations;}
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
        EventQueue<Actor>& get_event_queue() {return m_events;}
        DataBlock& get_data() {return m_data;}
        bool late_polling() const {return m_late_polling;}
        double get_angle() const {return m_angle;}
        void set_angle(double angle) {m_angle = angle;}
        void set_tile(Tile tile) {m_base_tile = tile;}
        bool valid_anim_state(AnimationType anim, Direction dir) const;
        bool valid_anim_state() const {return valid_anim_state(m_anim_state, m_direction);}
        bool is_valid() const {return m_base_tile.is_valid();}

        SDL_Rect get_hitbox(std::string type = "COLLIDE") const;
        const std::map<std::string, SDL_Rect> get_hitboxes() const;

    private:
        MapData* m_map;

        float m_x = 0; ///< Coordinate of lower left image corner
        float m_y = 0; ///< @warning LOWER left corner!
        unsigned m_width = 0;
        unsigned m_height = 0;
        std::string m_name;
        std::string m_type;

        double m_angle = 0.0;

        AnimationType m_anim_state = AnimationType::none; ///< Currently active animation
        Direction m_direction = Direction::none; ///< Current direction facing
        std::map<AnimationType, std::map<Direction, Tile>> m_animations; ///< 2D Map which stores all animation tiles
        Tile m_base_tile;

        EventCollection<Actor, Response> m_response; ///< EventCollection which yields events for response values

        EventQueue<Actor> m_events;

        DataBlock m_data; ///< This holds custom user values by string

        bool m_late_polling = false;
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
