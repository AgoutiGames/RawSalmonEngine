/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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

Texture TextureCache::get(std::string full_path) {
    if(!load(full_path)) {return m_empty_texture;}
    else {return m_textures.at(full_path);}
}
bool TextureCache::load(std::string full_path) {
    std::cout << full_path << "\n";
    if(has(full_path)) {return true;}

    Texture temp;
    if(temp.loadFromFile(m_renderer,full_path)) {
        m_textures[full_path] = temp;
        return true;
    }
    else {return false;}
}
bool TextureCache::has(std::string full_path) {
    return (m_textures.find(full_path) != m_textures.end());
}
