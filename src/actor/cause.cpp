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

 #include <string>

 #include "actor/actor.hpp"
 #include "map/tile.hpp"

 // Default constructor
Cause::Cause() :
 type{CauseType::none}
{

}

// Constructor for keypress
Cause::Cause(SDL_Keysym key) :
 type{CauseType::key}
{
    data.key = key;
}

// Constructor for tile
Cause::Cause(Tile* tile, int x, int y) :
 type{CauseType::tile}
{
    data.tile.pointer = tile;
    data.tile.x = x;
    data.tile.y = y;
}

// Constructor for actor
Cause::Cause(Actor* actor) :
 type{CauseType::actor}
{
    data.actor = actor;
}

/**
 * @brief Return the type of the cause
 * @return Type as a string
 * @note Only actor and tiles return a type,
 *       key returns an empty string
 */
std::string Cause::get_type() const {
    if(type == CauseType::tile) {
        return data.tile.pointer->get_type();
    }
    else if(type == CauseType::actor) {
        return data.actor->get_type();
    }
    else {
        return std::string{""};
    }
}

/**
 * @brief Return the position of the cause
 * @return X and Y-coordinates
 * @note Only actor and tiles return their coords,
 *       key returns 0, 0
 */
std::pair<int, int> Cause::get_pos() const {
    if(type == CauseType::tile) {
        return {data.tile.x, data.tile.y};
    }
    else if(type == CauseType::actor) {
        int x = data.actor->get_x();
        int y = data.actor->get_y() - data.actor->get_h();
        return {x, y};
    }
    else {return {0,0};}
}