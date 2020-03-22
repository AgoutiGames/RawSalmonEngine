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
#ifndef TEXTURE_CACHE_HPP_INCLUDED
#define TEXTURE_CACHE_HPP_INCLUDED

#include <string>
#include <map>

#include "graphics/texture.hpp"

class TextureCache {
    public:

        void init(SDL_Renderer* renderer) {m_renderer = renderer;}

        Texture get(std::string full_path);
        bool load(std::string full_path);

        Texture get(std::string full_path, SDL_Color color_key);
        bool load(std::string full_path, SDL_Color color_key);

        bool has(std::string full_path);

    private:
        SDL_Renderer* m_renderer;
        Texture m_empty_texture;
        std::map<std::string, Texture> m_textures;
};


#endif // TEXTURE_CACHE_HPP_INCLUDED
