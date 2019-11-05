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
#ifndef COLLISION_HPP_INCLUDED
#define COLLISION_HPP_INCLUDED

#include <string>
#include <SDL.h>

#include "util/game_types.hpp"

class Actor;
class Tile;

/**
 * @brief Store information of an actor collision
 *
 */

class Collision{
    private:
        enum class CollisionType{
            none,
            tile,
            actor,
            mouse,
        };

    public:
        Collision();
        Collision(std::string my_hitbox);
        Collision(Tile* tile, std::string my_hitbox, std::string other_hitbox);
        Collision(Actor* actor, std::string my_hitbox, std::string other_hitbox);

        // Checks against tile types
        bool tile() const {return type == CollisionType::tile;}
        bool actor() const {return type == CollisionType::actor;}
        bool mouse() const {return type == CollisionType::mouse;}
        bool none() const {return type == CollisionType::none;}

        std::string my_hitbox() const {return my_hitbox_name;}
        std::string other_hitbox() const {return other_hitbox_name;}

        // Return cause objects
        Actor* get_actor() const {return type == CollisionType::actor ? data.actor : nullptr;}
        Tile* get_tile() const {return type == CollisionType::tile ? data.tile : nullptr;}

        std::string get_type() const;

    private:
        CollisionType type;

        union{
            Actor* actor;
            Tile* tile;
        } data;

        std::string my_hitbox_name = "";
        std::string other_hitbox_name = "";

};


#endif // COLLISION_HPP_INCLUDED
