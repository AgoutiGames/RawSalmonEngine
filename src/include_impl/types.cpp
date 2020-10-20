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
#include "types.hpp"

#include <cmath>

namespace salmon {
PixelRect::PixelRect(Rect r) : x{static_cast<int>(std::round(r.x))},
                               y{static_cast<int>(std::round(r.y))},
                               w{static_cast<int>(std::round(r.w))},
                               h{static_cast<int>(std::round(r.h))} {}

PixelPoint::PixelPoint(Point p) : x{static_cast<int>(std::round(p.x))},
                                  y{static_cast<int>(std::round(p.y))} {}

PixelDimensions::PixelDimensions(Dimensions d) :
    w{static_cast<int>(std::round(d.w))},
    h{static_cast<int>(std::round(d.h))} {}

Point Point::move(Point p) {
    x += p.x;
    y += p.y;
    return *this;
}

Dimensions Dimensions::scale(Scale s) {
    w *= s.w;
    h *= s.h;
    return *this;
}

Rect Rect::scale(Scale s) {
    w *= s.w;
    h *= s.h;
    return *this;
}

Rect Rect::move(Point p) {
    x += p.x;
    y += p.y;
    return *this;
}

}
