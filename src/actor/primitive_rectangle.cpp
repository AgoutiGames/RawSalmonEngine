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
#include "actor/primitive_rectangle.hpp"

#include <SDL.h>

using namespace salmon::internal;

bool PrimitiveRectangle::render(int x_cam, int y_cam) const {
    if(m_hidden) {return true;}
    if(m_static_mode) {x_cam = 0; y_cam = 0;}
    SDL_Rect rect{static_cast<int>(m_x_pos-x_cam), static_cast<int>(m_y_pos-y_cam),m_width,m_height};

    return SDL_RenderDrawRect(m_renderer, &rect) == 0;
}
