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

#include "./types.hpp"
#include "./data_block_ref.hpp"

class Actor;

namespace salmon {
    class ActorRef {
        friend class CameraRef;
        public:
            ActorRef(Actor& impl);

            bool animate(salmon::AnimationType anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);
            bool set_animation(salmon::AnimationType anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, int frame = 0);
            salmon::AnimSignal animate_trigger(salmon::AnimationType anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);
            salmon::AnimationType get_animation() const;
            salmon::Direction get_direction() const;
            std::string get_name() const;
            std::string get_type() const;

            bool move(float x_factor, float y_factor, bool absolute = false);
            bool on_ground(salmon::Direction dir = salmon::Direction::down, int tolerance = 0) const;

            double get_angle() const;
            void set_angle(double angle);

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
