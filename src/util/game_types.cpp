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
#include "game_types.hpp"

/// Converts a @c string to an @c enum of @c AnimationType
AnimationType str_to_anim_type(const std::string& name) {
    if(name == "IDLE") return AnimationType::idle;
    if(name == "WALK") return AnimationType::walk;
    else return AnimationType::invalid;
}

/// Converts a @c string to an @c enum of @c Direction
Direction str_to_direction(const std::string& name) {
    if(name == "UP") return Direction::up;
    if(name == "DOWN") return Direction::down;
    if(name == "LEFT") return Direction::left;
    if(name == "RIGHT") return Direction::right;
    else return Direction::invalid;
}

/// Converts a @c string to an @c enum of @c Behaviour
Behaviour str_to_behaviour(const std::string& name) {
    if(name == "IDLE") return Behaviour::idle;
    if(name == "WALK_AROUND") return Behaviour::walk_around;
    else return Behaviour::invalid;
}

/// Converts a @c Direction to x and y factors
std::vector<float> dir_to_mov(const Direction dir) {
    if(dir == Direction::up)    return std::vector<float>{0,-1};
    if(dir == Direction::right) return std::vector<float>{1,0}
    if(dir == Direction::down)  return std::vector<float>{0,1}
    if(dir == Direction::left)  return std::vector<float>{-1,0}
    else return std::vector<float>{0,0}
}
