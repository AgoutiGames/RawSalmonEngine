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
#include "util/game_types.hpp"

namespace salmon { namespace internal {

bool PrimitiveRectangle::render(float x_cam, float y_cam) const {
    if(m_hidden) {return true;}
    Rect r = m_transform.to_rect();
    r.x -= x_cam;
    r.y -= y_cam;
    PixelRect p = r;
    SDL_Rect sdl_rect = make_rect(p);

    return SDL_RenderDrawRect(m_renderer, &sdl_rect) == 0;
}

}} // namespace salmon::internal
