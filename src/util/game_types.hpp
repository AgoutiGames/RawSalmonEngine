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
#ifndef GAME_TYPES_HPP_INCLUDED
#define GAME_TYPES_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>

#include "map/tile.hpp"


/**
 * @brief A collection of various enums
 */

enum class Behaviour {
    idle,
    walk_around,
    invalid,
};

enum class Direction {
    up,
    down,
    left,
    right,
    invalid,
};

enum class AnimationType {
    idle,
    walk,
    invalid,
};

enum class Priority{
    low,
    medium,
    high,
    clear_all,
};

struct ActorTemplate {
    std::string template_name = "_";
    float speed = 250.0f;
    Behaviour AI = Behaviour::idle;
    Direction direction = Direction::down;
    SDL_Rect hitbox = {0,0,0,0};
    std::map<AnimationType, std::map<Direction, Tile>> animations;
};

AnimationType str_to_anim_type(const std::string& name);
Direction str_to_direction(const std::string& name);
Behaviour str_to_behaviour(const std::string& name);

std::vector<float> dir_to_mov(const Direction dir);

#endif // GAME_TYPES_HPP_INCLUDED
