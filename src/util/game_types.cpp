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
#include "util/game_types.hpp"
#include <iostream>
#include <cmath>

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace salmon { namespace internal {

/// Converts a @c string to an @c enum of @c Direction
Direction str_to_direction(const std::string& name) {
    if(name == "UP") return Direction::up;
    if(name == "UP_RIGHT") return Direction::up_right;
    if(name == "RIGHT") return Direction::right;
    if(name == "DOWN_RIGHT") return Direction::down_right;
    if(name == "DOWN") return Direction::down;
    if(name == "DOWN_LEFT") return Direction::down_left;
    if(name == "LEFT") return Direction::left;
    if(name == "UP_LEFT") return Direction::up_left;
    if(name == "CURRENT") return Direction::current;
    if(name == "NONE") return Direction::none;
    else return Direction::invalid;
}

/// Converts a @c Direction to x and y factors
std::vector<float> dir_to_mov(const Direction dir) {
    if(dir == Direction::up)    return std::vector<float>{0,-1};
    if(dir == Direction::right) return std::vector<float>{1,0};
    if(dir == Direction::down)  return std::vector<float>{0,1};
    if(dir == Direction::left)  return std::vector<float>{-1,0};
    else return std::vector<float>{0,0};
}

/// Converts a @c string to an @c SDL_Color
/// @note Format is #RRGGBB or #AARRGGBB, or RRGGBB or AARRGGBB, no error checking!
SDL_Color str_to_color(const std::string& name) {
        std::string n = name;
        SDL_Color color;

        // Delete leading # sign if it's there
        if(*n.cbegin() == '#') {
            n.erase(n.begin());
        }

        // Check for possible alpha value (since ARGB and RGB is possible)
        if(n.length() > 6) {
            color.a = std::stoul(std::string(n, 0, 2), nullptr, 16);
            n.erase(n.begin(), n.begin() + 2);
        }
        // Set to fully opaque if no value is supplied
        else {color.a = 255;}

        color.r = std::stoul(std::string(n, 0, 2), nullptr, 16);
        color.g = std::stoul(std::string(n, 2, 2), nullptr, 16);
        color.b = std::stoul(std::string(n, 4, 2), nullptr, 16);

        return color;
}

void make_path_absolute(std::string& path) {
    path = fs::path(path).make_preferred().string();
    #ifndef __EMSCRIPTEN__
        path = fs::canonical(path).string();
    #endif
}

SDL_Point rect_center_difference(const SDL_Rect& first, const SDL_Rect& second) {
    SDL_Point first_center = {first.x + (first.w / 2), first.y + (first.h / 2)};
    SDL_Point second_center = {second.x + (second.w / 2), second.y + (second.h / 2)};
    return {second_center.x - first_center.x, second_center.y - first_center.y};
}

Rect make_rect(const SDL_Rect& rect) {
    return Rect{rect.x,rect.y,rect.w,rect.h};
}

SDL_Rect make_rect(const Rect& rect) {
    return SDL_Rect{rect.x,rect.y,rect.w,rect.h};
}

void normalize(float& x, float& y) {
    float len = std::sqrt(x*x + y*y);
    x /= len;
    y /= len;
}

}} // namespace salmon::internal
