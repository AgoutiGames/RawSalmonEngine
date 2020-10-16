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
#ifndef ACTOR_HPP_INCLUDED
#define ACTOR_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <tinyxml2.h>

#include "transform.hpp"
#include "actor/collision.hpp"
#include "actor/data_block.hpp"
#include "map/tile.hpp"
#include "util/game_types.hpp"

class MapData;

/**
 * @brief Parse, store and manage all actors
 *
 */
class Actor{

    public:
        Actor(MapData* map);

        // Core functions
        tinyxml2::XMLError parse_base(tinyxml2::XMLElement* source);
        tinyxml2::XMLError parse_properties(tinyxml2::XMLElement* source);

        bool animate(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);
        bool set_animation(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, int frame = 0);
        salmon::AnimSignal animate_trigger(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);

        void render(int x_cam, int y_cam) const;

        // DEPRECATED! Use move_relative and move_absolute instead!
        bool move(float x_factor, float y_factor, bool absolute = false);

        // Move with collision
        bool move_relative(float x, float y, salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        bool move_absolute(float x, float y, salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        // Move without collision
        void move_relative(float x, float y);
        void move_absolute(float x, float y);

        bool unstuck(salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);

        bool unstuck_along_path(float x, float y,salmon::Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);

        bool check_collision(Actor& other, bool notify);
        bool check_collision(Actor& other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);

        bool check_collision(TileInstance& other, bool notify);
        bool check_collision(TileInstance& other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);

        // DEPRECATED! Use more granular overload instead
        bool on_ground(salmon::Direction dir = salmon::Direction::down, int tolerance = 0) const {return on_ground(salmon::Collidees::tile, salmon::DEFAULT_HITBOX, {salmon::DEFAULT_HITBOX},dir,tolerance);}
        bool on_ground(salmon::Collidees target, std::string my_hitbox, const std::vector<std::string>& other_hitboxes, salmon::Direction dir = salmon::Direction::down, int tolerance = 0) const;

        // Seperate hitboxes after collision
        bool separate(TileInstance& tile, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        bool separate(Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        bool separate(const SDL_Rect& first, const SDL_Rect& second);

        // Separate hitboxes after collision restricted to one direction given in x y values
        bool separate_along_path(float x, float y,TileInstance& tile, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        bool separate_along_path(float x, float y,Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify);
        bool separate_along_path(float x, float y,const SDL_Rect& first, const SDL_Rect& second);

        // Separate this and another actor by supplied vectors each
        bool separate_along_path(float x1, float y1, float x2, float y2, Actor& actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes);

        Transform& get_transform() {return m_transform;}
        const Transform& get_transform() const {return m_transform;}

            // SOON TO BE REPLACED BY TRANSFORM
            // Return lower left corner coords
            float get_x() const {return m_transform.get_relative(0.0,1.0).first;}
            float get_y() const {return m_transform.get_relative(0.0,1.0).second;}
            float get_bottom() const {return m_transform.get_relative(0.0,1.0).second;}
            unsigned get_w() const {return m_transform.get_base_dimensions().first;}
            unsigned get_h() const {return m_transform.get_base_dimensions().second;}
            int get_x_center() const {return static_cast<int>(m_transform.get_relative(0.5,0.5).first);}
            int get_y_center() const {return static_cast<int>(m_transform.get_relative(0.5,0.5).second);}
            double get_angle() const {return m_transform.get_rotation();}
            void set_angle(double angle) {m_transform.set_rotation(angle);}
            bool scale(float x, float y) {m_transform.set_scale(x,y);return true;}
            bool scale(float s) {return scale(s,s);}
            float get_x_scale() const {return m_transform.get_scale().first;}
            float get_y_scale() const {return m_transform.get_scale().second;}
            SDL_Rect get_boundary() const {return m_transform.to_bounding_box();}

        void set_name(std::string name) {m_name = name;}

        // Trivial Getters
        std::string get_animation() const {return m_anim_state;}
        Tile& get_animation_tile() {return m_animations.at(m_anim_state).at(m_direction);}
        std::map<std::string, std::map<salmon::Direction, Tile>>& get_animation_container() {return m_animations;}
        salmon::Direction get_direction() const {return m_direction;}
        std::string get_name() const {return m_name;}
        std::string get_type() const {return m_type;}
        MapData& get_map() {return *m_map;}

        DataBlock& get_data() {return m_data;}
        bool late_polling() const {return m_late_polling;}

        void set_tile(Tile tile) {m_base_tile = tile;}

        bool valid_anim_state(std::string anim, salmon::Direction dir) const;
        bool valid_anim_state() const {return valid_anim_state(m_anim_state, m_direction);}

        bool is_valid() const {return m_base_tile.is_valid();}

        unsigned get_id() const {return m_id;}
        void set_id(unsigned id) {m_id = id;}

        bool get_hidden() const {return m_hidden;}
        void set_hidden(bool mode) {m_hidden = mode;}

        void set_layer(std::string layer) {m_layer_name = layer;}
        std::string get_layer() const {return m_layer_name;}

        bool get_resize_hitbox() const {return m_resize_hitbox;}
        void set_resize_hitbox(bool mode) {m_resize_hitbox = mode;}

        SDL_Rect get_hitbox(std::string type = salmon::DEFAULT_HITBOX) const;
        const std::map<std::string, SDL_Rect> get_hitboxes() const;

        void add_collision(Collision c) {if(m_register_collisions) {m_collisions.push_back(c);}}
        std::vector<Collision>& get_collisions() {return m_collisions;}
        void clear_collisions() {m_collisions.clear();}
        void register_collisions(bool r) {if(!r) {clear_collisions();} m_register_collisions = r;}

    private:
        void transform_hitbox(SDL_Rect& hitbox) const;

        MapData* m_map;

        Transform m_transform;

        std::string m_name;
        std::string m_type;
        std::string m_layer_name;

        std::string m_anim_state = salmon::AnimationType::none; ///< Currently active animation
        salmon::Direction m_direction = salmon::Direction::none; ///< Current direction facing
        std::map<std::string, std::map<salmon::Direction, Tile>> m_animations; ///< 2D Map which stores all animation tiles
        Tile m_base_tile;

        DataBlock m_data; ///< This holds custom user values by string

        std::vector<Collision> m_collisions;
        bool m_register_collisions = true;

        unsigned m_id = 0;

        bool m_late_polling = false;

        // If true the hitbox grows and shrinks with varying size
        bool m_resize_hitbox = true;

        bool m_hidden = false;
};

/// Overloading of comparision operator to enable the sort() functionality in a std::list
inline bool operator< (const Actor& lhs, const Actor& rhs) {
    auto lp = lhs.get_transform().get_sort_point();
    auto rp = rhs.get_transform().get_sort_point();

    if(lp.second != rp.second) {
        return lp.second < rp.second;
    }
    else {
        return lp.first < rp.first;
    }
}

#endif // ACTOR_HPP_INCLUDED
