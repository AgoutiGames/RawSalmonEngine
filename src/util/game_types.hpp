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
#ifndef GAME_TYPES_HPP_INCLUDED
#define GAME_TYPES_HPP_INCLUDED

#include <string>
#include <vector>
#include <SDL.h>
#include "../../include/types.hpp"

namespace salmon { namespace internal {

/**
 * @brief A collection of various enums and handy functions for conversion and parsing
 */

// AnimationType str_to_anim_type(const std::string& name);
Direction str_to_direction(const std::string& name);
SDL_Color str_to_color(const std::string& name);
void make_path_absolute(std::string& path);

SDL_Point rect_center_difference(const SDL_Rect& first, const SDL_Rect& second);
Rect make_rect(const SDL_Rect& rect);
SDL_Rect make_rect(const Rect& rect);

std::vector<float> dir_to_mov(const Direction dir);
void normalize(float& x, float& y);

}} // namespace salmon::internal

#endif // GAME_TYPES_HPP_INCLUDED
