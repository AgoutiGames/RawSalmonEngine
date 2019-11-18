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
#ifndef MAP_REF_HPP_INCLUDED
#define MAP_REF_HPP_INCLUDED

#include <vector>

#include "./actor_ref.hpp"
#include "./data_block_ref.hpp"
#include "./camera_ref.hpp"

class MapData;

namespace salmon {
    class MapRef {
        public:
            MapRef(MapData& impl);
            MapRef(MapData* impl);
            virtual ~MapRef() = default;

            bool render() const;
            void update();

            std::vector<salmon::ActorRef> get_actors();
            CameraRef get_camera();

            unsigned get_w() const;
            unsigned get_h() const;
            float get_delta_time() const;
            std::string get_path() const;
            DataBlockRef get_data();
        private:
            MapData* m_impl;
    };
}


#endif // MAP_REF_HPP_INCLUDED
