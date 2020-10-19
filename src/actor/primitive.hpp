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
        Primitive(float x_pos, float y_pos, MapData& mapdata, std::string name = "");
        virtual ~Primitive() = default;

        virtual bool render(int x_cam, int y_cam) const = 0;
        virtual PrimitiveType get_type() const {return PrimitiveType::undefined;}

        virtual Primitive* clone() const = 0;

        float get_x() {return m_x_pos;}
        float get_y() {return m_y_pos;}
        std::string get_name() {return m_name;}

        void set_x(float x) {m_x_pos = x;}
        void set_y(float y) {m_y_pos = y;}

        bool get_static_mode() const {return m_static_mode;}
        void set_static_mode(bool mode) {m_static_mode = mode;}

        bool get_hidden() const {return m_hidden;}
        void set_hidden(bool mode) {m_hidden = mode;}

        void move(float x, float y) {m_x_pos += x; m_y_pos += y;}

        static Primitive* parse(tinyxml2::XMLElement* source, MapData& base_map);
    protected:
        float m_x_pos;
        float m_y_pos;
        SDL_Renderer* m_renderer;
        std::string m_name;
        bool m_static_mode = false;
        bool m_hidden = false;
};
}} // namespace salmon::internal

#endif // PRIMITIVE_HPP_INCLUDED
