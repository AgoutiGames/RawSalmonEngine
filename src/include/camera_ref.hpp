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
#ifndef CAMERA_REF_HPP_INCLUDED
#define CAMERA_REF_HPP_INCLUDED

#include "./actor_ref.hpp"

class Camera;

namespace salmon {
    class CameraRef {
        public:
            CameraRef(Camera& impl);

            int get_x() const;
            int get_y() const;

            void set_x(int x);
            void set_y(int y);

            void bind_actor(ActorRef actor);
            void unbind_actor();

            void bind_map();
            void unbind_map();

        private:
            Camera* m_impl;
    };
}

#endif // CAMERA_REF_HPP_INCLUDED
