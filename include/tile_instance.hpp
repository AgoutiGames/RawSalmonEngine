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
#ifndef TILE_INSTANCE_HPP_INCLUDED
#define TILE_INSTANCE_HPP_INCLUDED

#include <string>
#include <memory>

#include "./types.hpp"

class TileInstance;

namespace salmon {
    class TileInstance {
        public:
            TileInstance(::TileInstance tile);

            /// Returns true if the tile exists
            bool valid() const;

            /// Returns the x coordinate of the upper left corner
            int get_x() const;
            /// Returns the y coordinate of the upper left corner
            int get_y() const;

            /// Returns the x coordinate of the tile center
            int get_x_center() const;
            /// Returns the y coordinate of the tile center
            int get_y_center() const;

            /// Returns the width of the tile
            unsigned get_w() const;
            /// Returns the height of the tile
            unsigned get_h() const;

            /// Returns the hitbox matching the supplied name. If there is none, w or h is zero.
            salmon::Rect get_hitbox(std::string name) const;

            /// Returns the type string of the tile
            std::string get_type() const;

        private:
            std::unique_ptr<::TileInstance> m_impl;
    };
}

#endif // TILE_INSTANCE_HPP_INCLUDED
