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
#include "graphics/texture_cache.hpp"

#include <iostream>

#include "util/game_types.hpp"

namespace salmon { namespace internal {

Texture TextureCache::get(std::string full_path) {
    make_path_absolute(full_path);
    if(!load(full_path)) {return m_empty_texture;}
    else {return m_textures.at(full_path);}
}
bool TextureCache::load(std::string full_path) {
    make_path_absolute(full_path);
    if(has(full_path)) {return true;}

    Texture temp;
    if(temp.loadFromFile(m_renderer,full_path)) {
        m_textures[full_path] = temp;
        return true;
    }
    else {return false;}
}

Texture TextureCache::get(std::string full_path, SDL_Color color_key) {
    make_path_absolute(full_path);
    if(!load(full_path, color_key)) {return m_empty_texture;}
    else {return m_textures.at(full_path);}
}
bool TextureCache::load(std::string full_path, SDL_Color color_key) {
    make_path_absolute(full_path);

    // Always load the image again if a color key is supplied
    // if(has(full_path)) {return true;}

    Texture temp;
    if(temp.loadFromFile(m_renderer,full_path,color_key)) {
        m_textures[full_path] = temp;
        return true;
    }
    else {return false;}
}

bool TextureCache::has(std::string full_path) {
    return (m_textures.find(full_path) != m_textures.end());
}

}} // namespace salmon::internal
