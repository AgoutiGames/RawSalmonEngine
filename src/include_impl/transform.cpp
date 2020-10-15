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

Transform::Transform(float x_pos, float y_pos, float width, float height, float x_origin, float y_origin)
    : m_x_pos{x_pos}, m_y_pos{y_pos}, m_width{width}, m_height{height}, m_x_origin{x_origin}, m_y_origin{y_origin} {}

void Transform::set_origin(float x, float y) {
    auto location = get_relative(x,y);
    m_x_pos = location.first;
    m_y_pos = location.second;
    m_x_origin = x;
    m_y_origin = y;
}

std::pair<float, float> Transform::get_relative(float x, float y) {
    float x_pos = m_x_pos;
    float y_pos = m_y_pos;
    // relative difference of origin to point
    x -= m_x_origin;
    y -= m_y_origin;
    x_pos += x * m_width * m_x_scale;
    y_pos += y * m_height * m_y_scale;
    return {x_pos,y_pos};
}

SDL_Rect Transform::to_rect() {
    int x,y,w,h;
    auto loc = get_relative(0,0);
    x = std::round(loc.first);
    y = std::round(loc.second);
    w = std::round(m_width * m_x_scale);
    h = std::round(m_height * m_y_scale);
    return SDL_Rect{x,y,w,h};
}
