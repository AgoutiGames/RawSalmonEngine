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

/// Converts a @c string to an @c enum of @c Direction
salmon::Direction str_to_direction(const std::string& name) {
    if(name == "UP") return salmon::Direction::up;
    if(name == "UP_RIGHT") return salmon::Direction::up_right;
    if(name == "RIGHT") return salmon::Direction::right;
    if(name == "DOWN_RIGHT") return salmon::Direction::down_right;
    if(name == "DOWN") return salmon::Direction::down;
    if(name == "DOWN_LEFT") return salmon::Direction::down_left;
    if(name == "LEFT") return salmon::Direction::left;
    if(name == "UP_LEFT") return salmon::Direction::up_left;
    if(name == "CURRENT") return salmon::Direction::current;
    if(name == "NONE") return salmon::Direction::none;
    else return salmon::Direction::invalid;
}

/// Converts a @c string to an @c enum of @c Priority
Priority str_to_priority(const std::string& name) {
    if(name == "LOW") return Priority::low;
    if(name == "MEDIUM") return Priority::medium;
    if(name == "HIGH") return Priority::high;
    else return Priority::invalid;
}

/// Converts a @c string to an @c enum of @c EventSignal
EventSignal str_to_event_signal(const std::string& name) {
    if(name == "NEXT") return EventSignal::next;
    if(name == "STOP") return EventSignal::stop;
    if(name == "END") return EventSignal::end;
    if(name == "ABORT") return EventSignal::abort;
    else return EventSignal::invalid;
}

/// Converts a @c string to an @c enum of @c Response
Response str_to_response(const std::string& name) {
    if(name == "ON_HIT") return Response::on_hit;
    if(name == "ON_COLLISION") return Response::on_collision;
    if(name == "ON_ACTIVATION") return Response::on_activation;
    if(name == "ON_DEATH") return Response::on_death;
    if(name == "ON_IDLE") return Response::on_idle;
    if(name == "ON_ALWAYS") return Response::on_always;
    if(name == "ON_SPAWN") return Response::on_spawn;
    if(name == "ON_MOUSE") return Response::on_mouse;
    else return Response::invalid;
}

/// Converts a @c Direction to x and y factors
std::vector<float> dir_to_mov(const salmon::Direction dir) {
    if(dir == salmon::Direction::up)    return std::vector<float>{0,-1};
    if(dir == salmon::Direction::right) return std::vector<float>{1,0};
    if(dir == salmon::Direction::down)  return std::vector<float>{0,1};
    if(dir == salmon::Direction::left)  return std::vector<float>{-1,0};
    else return std::vector<float>{0,0};
}

/// Converts a @c string to an @c SDL_Color
/// @note Format is #RRGGBB or #AARRGGBB, no error checking!
SDL_Color str_to_color(const std::string& name) {
        std::string n = name;
        SDL_Color color;
        n.erase(n.begin()); // Delete leading # sign

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
    path = fs::canonical(path).string();
}

SDL_Point rect_center_difference(const SDL_Rect& first, const SDL_Rect& second) {
    SDL_Point first_center = {first.x + (first.w / 2), first.y + (first.h / 2)};
    SDL_Point second_center = {second.x + (second.w / 2), second.y + (second.h / 2)};
    return {second_center.x - first_center.x, second_center.y - first_center.y};
}

salmon::Rect make_rect(const SDL_Rect& rect) {
    return salmon::Rect{rect.x,rect.y,rect.w,rect.h};
}

void normalize(float& x, float& y) {
    float len = std::sqrt(x*x + y*y);
    x /= len;
    y /= len;
}
