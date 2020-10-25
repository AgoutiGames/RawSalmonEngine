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

#include "./actor.hpp"
#include "./transform.hpp"

namespace salmon {

namespace internal{class Camera;}

class Camera {
    public:
        Camera(internal::Camera& impl);

        Transform& get_transform();

        /// Make camera always center on supplied actor
        void bind_actor(Actor actor);
        /// Stop camera from centering on actor
        void unbind_actor();

        /// Bind camera to map borders. It may not scroll over the world edges
        void bind_map();
        /// Ubind camera from map borders
        void unbind_map();

        /// Apply possible actor or map limitations
        void update();

    private:
        internal::Camera* m_impl;
};
}

#endif // CAMERA_REF_HPP_INCLUDED
