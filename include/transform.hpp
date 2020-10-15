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
#ifndef TRANSFORM_HPP_INCLUDED
#define TRANSFORM_HPP_INCLUDED

#include "SDL.h"
#include <utility> // For std::pair

// Rectangular area easy to transform
class Transform{
    public:
        // Generate transform with origin located at the lower left corner
        Transform(float x_pos, float y_pos, float width, float height, float x_origin = 0.0f, float y_origin = 1.0);

        void set_pos(float x, float y) {m_x_pos = x; m_y_pos = y;}
        void move_pos(float x, float y) {m_x_pos += x; m_y_pos += y;}
        void set_dimensions(float w, float h) {m_width=w;m_height=h;}
        // Set origin using relative coordinates wit origin in upper left corner
        void set_origin(float x, float y);

        // Set scale to absolute values
        void set_scale(float x, float y) {m_x_scale = x; m_y_scale = y;};
        // Scale relative to current scale
        void scale(float x, float y) {set_scale(m_x_scale * x, m_y_scale * y);}

        void set_rotation(double angle) {m_angle = angle;}
        void rotate(double angle) {m_angle += angle;}

        double get_rotation() {return m_angle;}

        // Get position at normalized coordinates x and y relative to upper left corner of transform
        // Example: 0.0,0.0 Upper left corner; 0.5,0.5 middle point; 1.0,1.0 lower right corner
        std::pair<float, float> get_relative(float x, float y);

        // Generate rect with position relative to upper left corner and casted to int
        SDL_Rect to_rect();

    private:
        float m_x_pos;
        float m_y_pos;
        float m_width;
        float m_height;
        float m_x_scale = 1.0f;
        float m_y_scale = 1.0f;
        double m_angle = 0.0;

        float m_x_origin;
        float m_y_origin;
};

#endif // TRANSFORM_HPP_INCLUDED
