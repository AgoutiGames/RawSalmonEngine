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
#ifndef PRIMITIVE_RECTANGLE_HPP_INCLUDED
#define PRIMITIVE_RECTANGLE_HPP_INCLUDED

#include <string>

#include "actor/primitive.hpp"

namespace salmon { namespace internal {

class PrimitiveRectangle : public Primitive {
    public:
        PrimitiveRectangle(float x_pos, float y_pos, int width, int height, MapData& mapdata, std::string name = "")
                          : Primitive(x_pos,y_pos,mapdata,name), m_width{width}, m_height{height} {}

        bool render(int x_cam, int y_cam) const override;
        PrimitiveType get_type() const override {return PrimitiveType::rectangle;}

        // Covariant return type!
        PrimitiveRectangle* clone() const override {return new PrimitiveRectangle(*this);}

        static PrimitiveRectangle* parse(tinyxml2::XMLElement* source, MapData& base_map);
    private:
        int m_width;
        int m_height;
};
}} // namespace salmon::internal

#endif // PRIMITIVE_RECTANGLE_HPP_INCLUDED
