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
#include "transform.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "util/logger.hpp"

using namespace salmon::internal;

const float Transform::MIN_SCALE = 0.001;
const float Transform::MIN_ROTATION = 0.01;

Transform::Transform(float x_pos, float y_pos, float width, float height, float x_origin, float y_origin)
    : m_x_pos{x_pos}, m_y_pos{y_pos}, m_width{width}, m_height{height}, m_x_origin{x_origin}, m_y_origin{y_origin} {}

void Transform::set_origin(float x, float y) {
    auto location = get_relative(x,y);
    m_x_pos = location.first;
    m_y_pos = location.second;
    m_x_origin = x;
    m_y_origin = y;
}

std::pair<float, float> Transform::get_relative(float x, float y) const {
    float x_pos = m_x_pos;
    float y_pos = m_y_pos;
    // relative difference of origin to point
    x -= m_x_origin;
    y -= m_y_origin;
    x_pos += x * m_width * m_x_scale;
    y_pos += y * m_height * m_y_scale;
    return {x_pos,y_pos};
}

std::pair<float, float> Transform::get_relative_rotated(float x, float y) const {
    auto p1 = get_relative(m_x_rotate,m_y_rotate);
    auto p2 = get_relative(x,y);
    // Rotate p2 around p1
    double angle = m_angle * (M_PI / 180); // Angle in Radians
    float s = std::sin(angle);
    float c = std::cos(angle);
    p2.first -= p1.first;
    p2.second -= p1.second;
    float xnew = p2.first * c - p2.second * s;
    float ynew = p2.first * s + p2.second * c;
    p2.first = xnew + p1.first;
    p2.second = ynew + p1.second;
    return p2;
}

SDL_Rect Transform::to_rect() const {
    int x,y,w,h;
    auto loc = get_relative(0,0);
    x = std::round(loc.first);
    y = std::round(loc.second);
    w = std::round(m_width * m_x_scale);
    h = std::round(m_height * m_y_scale);
    return SDL_Rect{x,y,w,h};
}

void Transform::transform_hitbox(SDL_Rect& hitbox) const {
    float rel_x, rel_y;
    rel_x = hitbox.x / m_width;
    rel_y = hitbox.y / m_height;
    auto pos = get_relative(rel_x,rel_y);
    hitbox.x = std::round(pos.first);
    hitbox.y = std::round(pos.second);
    hitbox.w = std::round(hitbox.w * m_x_scale);
    hitbox.h = std::round(hitbox.h * m_y_scale);
}

void Transform::set_rotation_center(float x, float y) {
    auto p1 = get_relative(m_x_rotate,m_y_rotate);
    auto p2 = get_relative(x,y);
    std::pair<float,float> p3;
    // Rotate new center p2 around old center p1
    double angle = m_angle * (M_PI / 180); // Angle in Radians
    float s = std::sin(angle);
    float c = std::cos(angle);
    p2.first -= p1.first;
    p2.second -= p1.second;
    float xnew = p2.first * c - p2.second * s;
    float ynew = p2.first * s + p2.second * c;
    p2.first = xnew + p1.first;
    p2.second = ynew + p1.second;

    // Now rotate p1 around the new point, forming p3
    angle = -m_angle * (M_PI / 180); // Angle in Radians
    s = std::sin(angle);
    c = std::cos(angle);
    p3 = p1;
    p3.first -= p2.first;
    p3.second -= p2.second;
    xnew = p3.first * c - p3.second * s;
    ynew = p3.first * s + p3.second * c;
    p3.first = xnew + p2.first;
    p3.second = ynew + p2.second;

    // Calculate the translation vector, flip y-translation
    move_pos(p3.first - p1.first, p3.second - p1.second);

    // Formally set new rotation point
    m_x_rotate = x;
    m_y_rotate = y;
}

bool Transform::is_scaled() const {
    if(std::abs(1.0f-m_x_scale) > MIN_SCALE || std::abs(1.0f-m_y_scale) > MIN_SCALE) {
        return true;
    }
    else {
        return false;
    }
}

bool Transform::is_rotated() const {
    if(std::abs(m_angle) > MIN_ROTATION) {
        return true;
    }
    else {
        return false;
    }
}

SDL_Rect Transform::to_bounding_box() const {
    if(!is_rotated()) {
        return to_rect();
    }
    else {
        auto p1 = get_relative_rotated(0,0);
        auto p2 = get_relative_rotated(0,1);
        auto p3 = get_relative_rotated(1,0);
        auto p4 = get_relative_rotated(1,1);
        std::vector<float> x = {p1.first,p2.first,p3.first,p4.first};
        std::vector<float> y = {p1.second,p2.second,p3.second,p4.second};
        float minx = *std::min_element(x.begin(),x.end());
        float maxx = *std::max_element(x.begin(),x.end());
        float miny = *std::min_element(y.begin(),y.end());
        float maxy = *std::max_element(y.begin(),y.end());
        return SDL_Rect{static_cast<int>(std::round(minx)),
                        static_cast<int>(std::round(miny)),
                        static_cast<int>(std::round(maxx-minx)),
                        static_cast<int>(std::round(maxy-miny))};
    }
}

std::pair<float, float> Transform::get_relative_bounding_box(float x, float y) const {
    SDL_Rect bb = to_bounding_box();
    return {bb.x + x * bb.w, bb.y + y * bb.h};
}

std::pair<float, float> Transform::get_sort_point() const {
    if(m_sort_mode == SortMode::base || !is_rotated()) {
        return get_relative(m_x_sort,m_y_sort);
    }
    else if(m_sort_mode == SortMode::rotated) {
        return get_relative_rotated(m_x_sort,m_y_sort);
    }
    else if(m_sort_mode == SortMode::bounding_box) {
        return get_relative_bounding_box(m_x_sort,m_y_sort);
    }
    else {
        Logger(Logger::error) << "There is no sort mode nr:" << m_sort_mode << " did you forget to add it to Transform::get_sort_point()?";
        return {0,0};
    }
}

