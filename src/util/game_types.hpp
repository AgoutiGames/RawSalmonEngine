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

#include "graphics/texture.hpp"
#include "util/tinyxml2.h"

class ActorEvent;
class Tile;

/**
 * @brief A collection of various enums
 */

enum class Direction {
    up,
    down,
    left,
    right,
    current,
    invalid,
};

enum class AnimationType {
    idle,
    walk,
    current,
    invalid,
};

enum class Priority{
    low,
    medium,
    high,
    invalid,
};

enum class EventSignal{
    next,
    stop,
    end,
    die,
    invalid,
};

enum class Response{
    on_hit,
    on_collision,
    on_activation,
    on_death,
    on_idle,
    invalid,
};

enum class AnimSignal{
    trigger,
    wrap,
    next,
    none,
};

struct ActorTemplate {
    std::string template_name = "_";
    float speed = 250.0f; // Pixel per second
    Direction direction = Direction::down;
    std::map<std::string, SDL_Rect> hitbox;
    std::map<AnimationType, std::map<Direction, Tile>> animations;
    std::map<Response, ActorEvent*> response; ///< Map which yields events for response values
};

AnimationType str_to_anim_type(const std::string& name);
Direction str_to_direction(const std::string& name);
Priority str_to_priority(const std::string& name);
EventSignal str_to_event_signal(const std::string& name);
Response str_to_response(const std::string& name);
tinyxml2::XMLError parse_hitbox(tinyxml2::XMLElement* source, SDL_Rect& rect);
tinyxml2::XMLError parse_hitboxes(tinyxml2::XMLElement* source, std::map<std::string, SDL_Rect>& rects);
tinyxml2::XMLError parse_blendmode(tinyxml2::XMLElement* source, Texture& img);

std::vector<float> dir_to_mov(const Direction dir);

#endif // GAME_TYPES_HPP_INCLUDED
