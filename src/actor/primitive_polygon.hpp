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
#ifndef PRIMITIVE_POLYGON_HPP_INCLUDED
#define PRIMITIVE_POLYGON_HPP_INCLUDED

#include "actor/primitive.hpp"

namespace salmon { namespace internal {

class PrimitivePolygon : public Primitive {
    public:
        bool render(float x_cam, float y_cam) const override;
        PrimitiveType get_type() const override {return PrimitiveType::polygon;}

        // Covariant return type!
        PrimitivePolygon* clone() const override {return new PrimitivePolygon(*this);}

        static PrimitivePolygon* parse(tinyxml2::XMLElement* source, MapData& base_map);
    private:

};
}} // namespace salmon::internal

#endif // PRIMITIVE_POLYGON_HPP_INCLUDED
