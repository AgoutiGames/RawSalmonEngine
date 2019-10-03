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
#ifndef PRIMITIVE_LINE_HPP_INCLUDED
#define PRIMITIVE_LINE_HPP_INCLUDED

#include "actor/primitive.hpp"

class PrimitiveLine : public Primitive {
    public:
        bool render(int x_cam, int y_cam) const override;
        PrimitiveType get_type() const override {return PrimitiveType::line;}

        // Covariant return type!
        PrimitiveLine* clone() const override {return new PrimitiveLine(*this);}

        static PrimitiveLine* parse(tinyxml2::XMLElement* source, MapData& base_map);
    private:

};

#endif // PRIMITIVE_LINE_HPP_INCLUDED
