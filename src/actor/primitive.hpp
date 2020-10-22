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
#ifndef PRIMITIVE_HPP_INCLUDED
#define PRIMITIVE_HPP_INCLUDED

#include <string>
#include <SDL.h>

#include "tinyxml2.h"

#include "transform.hpp"

namespace salmon { namespace internal {

class MapData;

class Primitive {
    public:
        enum PrimitiveType {
            rectangle,
            ellipse,
            line,
            polygon,
            point,
            tile,
            text,
            undefined,
        };
        Primitive(MapData& mapdata, std::string name = "");
        virtual ~Primitive() = default;

        virtual bool render(float x_cam, float y_cam) const = 0;
        virtual PrimitiveType get_type() const {return PrimitiveType::undefined;}

        virtual Primitive* clone() const = 0;

        Transform& get_transform() {return m_transform;}
        std::string get_name() {return m_name;}

        bool get_hidden() const {return m_hidden;}
        void set_hidden(bool mode) {m_hidden = mode;}

        static Primitive* parse(tinyxml2::XMLElement* source, MapData& base_map);
    protected:
        Transform m_transform;
        SDL_Renderer* m_renderer;
        std::string m_name;
        bool m_hidden = false;
};
}} // namespace salmon::internal

#endif // PRIMITIVE_HPP_INCLUDED
