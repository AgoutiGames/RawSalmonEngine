/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#ifndef PARSE_HPP_INCLUDED
#define PARSE_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>

#include "util/game_types.hpp"
#include "util/tinyxml2.h"

class Texture;
class Actor;
class MapData;
class GameInfo;

template <class EventType>
class PropertyListener;

namespace parse{
    tinyxml2::XMLError hitbox(tinyxml2::XMLElement* source, SDL_Rect& rect);
    tinyxml2::XMLError hitboxes(tinyxml2::XMLElement* source, std::map<std::string, SDL_Rect>& rects);
    tinyxml2::XMLError blendmode(tinyxml2::XMLElement* source, Texture& img);

    tinyxml2::XMLError bg_color(tinyxml2::XMLElement* source, SDL_Color& color);
}

#endif // PARSE_HPP_INCLUDED
