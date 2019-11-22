/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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
 #include "actor/collision.hpp"

 #include "actor/actor.hpp"
 #include "map/tile.hpp"

 // Default constructor
Collision::Collision() :
 type{CollisionType::none}
{

}

// Constructor for tile
Collision::Collision(Tile* tile, std::string my_hitbox, std::string other_hitbox) :
 type{CollisionType::tile}
{
    data.tile = tile;
    my_hitbox_name = my_hitbox;
    other_hitbox_name = other_hitbox;
}

// Constructor for actor
Collision::Collision(Actor* actor, std::string my_hitbox, std::string other_hitbox) :
 type{CollisionType::actor}
{
    data.actor = actor;
    my_hitbox_name = my_hitbox;
    other_hitbox_name = other_hitbox;
    actor_id = actor->get_id();
}

// Constructor for mouse
Collision::Collision(std::string my_hitbox) :
 type{CollisionType::mouse}
{
    my_hitbox_name = my_hitbox;
}

/**
 * @brief Return the type of the cause
 * @return Type as a string
 * @note Only actor and tiles return a type,
 *       key returns an empty string
 */
std::string Collision::get_type() const {
    if(type == CollisionType::tile) {
        return data.tile->get_type();
    }
    else if(type == CollisionType::actor) {
        return data.actor->get_type();
    }
    else {
        return std::string{""};
    }
}

unsigned Collision::get_actor_id() const {return actor_id;}

