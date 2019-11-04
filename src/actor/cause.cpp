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
 #include "actor/cause.hpp"

 #include "actor/actor.hpp"
 #include "map/tile.hpp"

 // Default constructor
Cause::Cause() :
 type{CauseType::none}
{

}

// Constructor for tile
Cause::Cause(Tile* tile, std::string my_hitbox, std::string other_hitbox) :
 type{CauseType::tile}
{
    data.tile = tile;
    my_hitbox_name = my_hitbox;
    other_hitbox_name = other_hitbox;
}

// Constructor for actor
Cause::Cause(Actor* actor, std::string my_hitbox, std::string other_hitbox) :
 type{CauseType::actor}
{
    data.actor = actor;
    my_hitbox_name = my_hitbox;
    other_hitbox_name = other_hitbox;
}

// Constructor for mouse
Cause::Cause(std::string my_hitbox) :
 type{CauseType::mouse}
{
    my_hitbox_name = my_hitbox;
}

/**
 * @brief Return the type of the cause
 * @return Type as a string
 * @note Only actor and tiles return a type,
 *       key returns an empty string
 */
std::string Cause::get_type() const {
    if(type == CauseType::tile) {
        return data.tile->get_type();
    }
    else if(type == CauseType::actor) {
        return data.actor->get_type();
    }
    else {
        return std::string{""};
    }
}

