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
#include "actor/primitive.hpp"

#include "map/mapdata.hpp"
#include "actor/primitive_rectangle.hpp"
#include "actor/primitive_ellipse.hpp"
#include "actor/primitive_line.hpp"
#include "actor/primitive_point.hpp"
#include "actor/primitive_polygon.hpp"
#include "actor/primitive_text.hpp"
#include "actor/primitive_tile.hpp"

Primitive::Primitive(float x_pos, float y_pos, MapData& mapdata, std::string name)
                    : m_x_pos{x_pos}, m_y_pos{y_pos}, m_renderer{mapdata.get_renderer()}, m_name{name} {}

Primitive* Primitive::parse(tinyxml2::XMLElement* source, MapData& base_map) {
    if(source->FirstChildElement("text") != nullptr) {
        return PrimitiveText::parse(source, base_map);
    }
    else {
        return nullptr;
    }
}
