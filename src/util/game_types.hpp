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

Point rect_center_difference(const Rect& first, const Rect& second);
PixelRect make_rect(const SDL_Rect& rect);
SDL_Rect make_rect(const PixelRect& rect);

std::vector<float> dir_to_mov(const Direction dir);
void normalize(float& x, float& y);

// Return true if line1 and line2 overlap
template<class Type>
static bool intersect(Type min1, Type max1, Type min2, Type max2) {
    // Determine greates minimum and smalles maximum
    if(min1 < min2) {min1 = min2;}
    if(max1 > max2) {max1 = max2;}
    if(min1 >= max1) {return false;}
    else {return true;}
}
// Create new line from line1 and line2 overlap
template<class Type>
static void intersect(Type min1, Type max1, Type min2, Type max2, Type& dst, Type& dst_offset) {
    // Determine greates minimum and smalles maximum
    if(min1 < min2) {min1 = min2;}
    if(max1 > max2) {max1 = max2;}
    dst = min1;
    dst_offset = max1 - min1;
}

}} // namespace salmon::internal

#endif // GAME_TYPES_HPP_INCLUDED
