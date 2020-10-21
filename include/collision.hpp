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
#ifndef COLLISION_REF_HPP_INCLUDED
#define COLLISION_REF_HPP_INCLUDED

#include <string>

#include "tile_instance.hpp"

namespace salmon {

namespace internal{class Collision;}

/**
 * @brief Reference to collision object. Identifies the collidee of an collision no matter if it's a tile, actor or mouse.
 */
class Collision {
    public:
        Collision(internal::Collision& impl);

        /// Return true if collidee is a tile
        bool tile() const;
        /// Return true if collidee is an actor
        bool actor() const;
        /// Return true if collidee is the mouse
        bool mouse() const;
        /// Return true if collidee is not defined
        bool none() const;

        /// Return the name of the hitbox of the collision "receiver"
        std::string my_hitbox() const;
        /// Return the name of the hitbox of the collision "giver"
        std::string other_hitbox() const;

        /// Return the actor id of the collidee if it is of type actor, otherwise return 0
        unsigned get_actor_id() const;

        /// Returns the instance of the tile collided with
        TileInstance get_tile() const;

        const Transform& get_transform() const;

    private:
        internal::Collision* m_impl;
};
}

#endif // COLLISION_REF_HPP_INCLUDED
