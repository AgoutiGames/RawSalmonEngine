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

#include "util/game_types.hpp"

namespace salmon {

int round(float number) {
    return static_cast<int>(std::round(number));
}

PixelRect::PixelRect(const Rect& r) : x{static_cast<int>(std::round(r.x))},
                               y{static_cast<int>(std::round(r.y))},
                               w{static_cast<int>(std::round(r.w))},
                               h{static_cast<int>(std::round(r.h))} {}

PixelPoint::PixelPoint(const Point& p) : x{static_cast<int>(std::round(p.x))},
                                  y{static_cast<int>(std::round(p.y))} {}

PixelDimensions::PixelDimensions(const Dimensions& d) :
    w{static_cast<int>(std::round(d.w))},
    h{static_cast<int>(std::round(d.h))} {}

Point Point::move(const Point& p) {
    x += p.x;
    y += p.y;
    return *this;
}

Dimensions Dimensions::scale(const Scale& s) {
    w *= s.w;
    h *= s.h;
    return *this;
}

Rect Rect::scale(const Scale& s) {
    w *= s.w;
    h *= s.h;
    return *this;
}

Rect Rect::move(const Point& p) {
    x += p.x;
    y += p.y;
    return *this;
}

bool Rect::has_intersection(const Rect& r) const {
    if(empty() || r.empty()
       || !internal::intersect(x, x+w, r.x, r.x+r.w)
       || !internal::intersect(y, y+h, r.y, r.y+r.h)) {return false;}
    else {return true;}
}

bool Rect::has_intersection(const Point& p) const {
    if(!empty() && p.x >= x && p.x <= x+w && p.y >= y && p.y <= y+h) {return true;}
    else {return false;}
}

Rect Rect::get_intersection(const Rect& r) const  {
    Rect temp{0,0,0,0};
    internal::intersect(x, x+w, r.x, r.x+r.w,temp.x,temp.w);
    internal::intersect(y, y+h, r.y, r.y+r.h,temp.y,temp.h);
    return temp;
}

PixelRect PixelRect::scale(const Scale& s) {
    w = static_cast<int>(std::round(s.w * w));
    h = static_cast<int>(std::round(s.h * h));
    return *this;
}

PixelRect PixelRect::move(const PixelPoint& p) {
    x += p.x;
    y += p.y;
    return *this;
}

bool PixelRect::has_intersection(const PixelRect& r) const {
    if(empty() || r.empty()
       || !internal::intersect(x, x+w, r.x, r.x+r.w)
       || !internal::intersect(y, y+h, r.y, r.y+r.h)) {return false;}
    else {return true;}
}

bool PixelRect::has_intersection(const PixelPoint& p) const {
    if(!empty() && p.x >= x && p.x <= x+w && p.y >= y && p.y <= y+h) {return true;}
    else {return false;}
}

PixelRect PixelRect::get_intersection(const PixelRect& r) const  {
    PixelRect temp{0,0,0,0};
    internal::intersect(x, x+w, r.x, r.x+r.w,temp.x,temp.w);
    internal::intersect(y, y+h, r.y, r.y+r.h,temp.y,temp.h);
    return temp;
}

} // namespace salmon

