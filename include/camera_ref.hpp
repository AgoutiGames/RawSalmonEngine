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
#ifndef CAMERA_REF_HPP_INCLUDED
#define CAMERA_REF_HPP_INCLUDED

#include "./actor_ref.hpp"

class Camera;

namespace salmon {
    class CameraRef {
        public:
            CameraRef(Camera& impl);

            /// Return x coordinate of upper left corner of camera in world space
            int get_x() const;
            /// Return y coordinate of upper left corner of camera in world space
            int get_y() const;

            /// Set x coordinate of upper left corner of camera in world space
            void set_x(int x);
            /// Set y coordinate of upper left corner of camera in world space
            void set_y(int y);

            /// Make camera always center on supplied actor
            void bind_actor(ActorRef actor);
            /// Stop camera from centering on actor
            void unbind_actor();

            /// Bind camera to map borders. It may not scroll over the world edges
            void bind_map();
            /// Ubind camera from map borders
            void unbind_map();

            /// Apply possible actor or map limitations
            void update();

        private:
            Camera* m_impl;
    };
}

#endif // CAMERA_REF_HPP_INCLUDED
