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
#ifndef ACTOR_REF_HPP_INCLUDED
#define ACTOR_REF_HPP_INCLUDED

#include <string>
#include <vector>

#include "./types.hpp"
#include "./collision_ref.hpp"
#include "./data_block_ref.hpp"

class Actor;

namespace salmon {
    class ActorRef {
        friend class CameraRef;
        friend class MapRef;
        public:
            ActorRef(Actor& impl);
            ActorRef(Actor* impl);
            virtual ~ActorRef() = default;

            bool good() const {return (m_impl == nullptr) ? false : true ;}

            bool animate(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);
            bool set_animation(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, int frame = 0);
            salmon::AnimSignal animate_trigger(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);
            std::string get_animation() const;
            salmon::Direction get_direction() const;
            std::string get_name() const;
            std::string get_type() const;
            unsigned get_id() const;

            bool move(float x_factor, float y_factor, bool absolute = false);
            bool on_ground(salmon::Direction dir = salmon::Direction::down, int tolerance = 0) const;

            bool scale(float x, float y);
            bool scale(float s);

            double get_angle() const;
            void set_angle(double angle);

            int get_current_anim_frame() const;
            int get_anim_frame_count() const;

            std::vector<CollisionRef> get_collisions();
            void clear_collisions();
            void register_collisions(bool r);

            /// @warning The static mode is broken/bugged! Use camera relative movement instead
            bool get_static_mode() const;
            /// @warning The static mode is broken/bugged! Use camera relative movement instead
            void set_static_mode(bool mode);

            bool get_hidden() const;
            void set_hidden(bool mode);

            std::string get_layer() const;

            float get_x() const;
            float get_y() const;
            unsigned get_w() const;
            unsigned get_h() const;
            int get_x_center() const;
            int get_y_center() const;

            DataBlockRef get_data();

        private:
            Actor* m_impl;
    };
}

#endif // ACTOR_REF_HPP_INCLUDED
