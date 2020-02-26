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

            /// Returns true if the actor exists / could be found
            bool good() const;

            /**
             * @brief Lets the actor play an animation sequence
             * @param anim The name of the type of animation to be played
             * @param dir The direction of the animation
             * @param speed A modifier for speed which even may be negative
             * @return true if animation wrapped around, false if not
             *
             * @note If animation type and direction combination doesn't exist, the error isn't indicated,
             *       and the animation stays at its prior state
             */
            bool animate(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);

            /**
             * @brief Sets animation to specific frame
             * @param anim The name of the type of animation to be played
             * @param dir The direction of the animation
             * @param frame The number of frame which is set
             * @return false if animation type and direction combination doesn't exist or the frame is out of bounds, true otherwise
             *
             * @note If animation type and direction combination doesn't exist,
             *       the animation stays at its prior state
             */
            bool set_animation(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, int frame = 0);

            /**
             * @brief Lets the actor play an animation sequence
             * @param anim The name of the type of animation to be played
             * @param dir The direction of the animation
             * @param speed A modifier for speed which even may be negative
             * @return an AnimSignal value indicating missing animation, reaching next frame, wrap around, or hitting a trigger frame
             *
             * @note If animation type and direction combination doesn't exist,
             *       and the animation stays at its prior state
             */
            salmon::AnimSignal animate_trigger(std::string anim = salmon::AnimationType::current, salmon::Direction dir = salmon::Direction::current, float speed = 1.0);

            /// @todo Add anim_is_valid
            /// @todo Fix on_ground method
            /// @todo Add new movement and collision methods

            /// Returns the type string of the currently active animation
            std::string get_animation() const;
            /// Returns the direction value of the currently active animation
            salmon::Direction get_direction() const;

            /// Returns the name of the actor
            std::string get_name() const;
            /// Returns the name of the actor template from which the actor got constructed
            std::string get_template_name() const;
            /// Returns the unique id of the actor
            unsigned get_id() const;

            bool move(float x_factor, float y_factor, bool absolute = false);
            bool on_ground(salmon::Direction dir = salmon::Direction::down, int tolerance = 0) const;

            /**
             * @brief Scales actors width and height
             * @param x The width scale which must be greater than 0.0
             * @param y The height scale which must be greater than 0.0
             * @return false if a scale less or equal zero is entered, true otherwise
             *
             * @warning When scaling greater than 1.0, pop in artifacts will occur due to offscreen tile culling
             */
            bool scale(float x, float y);
            /// Scales actor proportionally. See overload for details
            bool scale(float s);
            /// Return current x scale value
            float get_x_scale() const;
            /// Return current y scale value
            float get_y_scale() const;

            /// When set to true, hitboxes scale proportionally when the actor itself is scaled
            void set_resize_hitbox(bool mode);
            /// Returns if hitboxes scale with actor. True by default.
            bool get_resize_hitbox() const;

            /// Returns the angle at which this actor is displayed. Zero by default
            double get_angle() const;
            /// Sets the displaying angle in degrees rotating from the actors center
            void set_angle(double angle);

            /// Returns the number of the currently active animation frame
            int get_current_anim_frame() const;
            /// Returns the number of frames the currently active animation type direction combination has
            int get_anim_frame_count() const;

            /// Returns vector containing all collisions since last clear_collisions() call
            std::vector<CollisionRef> get_collisions();
            /// Clears actor of its detected collisions
            void clear_collisions();

            /// When set to false, collisions won't be stored and directly discarded, when true they will be stored again
            void register_collisions(bool r);

            /// Returns true if actor is currently hidden, false otherwise
            bool get_hidden() const;
            /// When mode is true, rendering will be suspended, when false actor will be rendered again
            void set_hidden(bool mode);

            /// Returns the name of the layer in which the actor resides
            std::string get_layer() const;

            /// Returns the x coordinate of the actor from its lower left corner
            float get_x() const;
            /// Returns the x coordinate of the actor from its lower left corner
            float get_y() const;
            /// Returns the width of the actor in pixels
            unsigned get_w() const;
            /// Returns the height of the actor in pixels
            unsigned get_h() const;
            /// Returns the x coordinate of the actor from its center
            int get_x_center() const;
            /// Returns the y coordinate of the actor from its center
            int get_y_center() const;

            /// Returns reference to DataBlock of this actor which holds all property values supplied via tiled
            DataBlockRef get_data();

        private:
            Actor* m_impl;
    };
}

#endif // ACTOR_REF_HPP_INCLUDED
