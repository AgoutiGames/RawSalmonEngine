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
#ifndef LAYER_HPP_INCLUDED
#define LAYER_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "actor/actor.hpp"
#include "graphics/texture.hpp"
#include "map/tileset.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"


/**
 * @brief Parse, store and draw individual layers of the game map
 *
 * Can store all possible layer types
 */

class Layer{
    private:
        enum LayerType{
            map,
            object,
            image,
            undefinied
        };

    public:
        Layer(unsigned tile_w, unsigned tile_h);

        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        bool render(SDL_Rect* camera) const;
        void update();

    private:
        LayerType m_type = undefinied;

        // members for m_type map
        std::string m_name;
        unsigned m_width;
        unsigned m_height;
        unsigned m_tile_w;
        unsigned m_tile_h;
        std::vector<std::vector<Uint16> > m_map_grid; ///< The actual map layer information
        float m_opacity = 1.0f; ///< @warning value has no effect yet!
        int m_offset_x = 0;
        int m_offset_y = 0;

        // members for m_type image
        std::string m_img_src;
        Texture m_img;

        // members for m_type object
        std::vector<Actor> m_obj_grid;

        static std::string m_base_path;

};


#endif // LAYER_HPP_INCLUDED