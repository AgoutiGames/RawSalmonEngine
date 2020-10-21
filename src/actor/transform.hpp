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

#include <utility> // For std::pair

#include "./types.hpp"

namespace salmon { namespace internal {

// Rectangular area easy to transform
class Transform{
    public:
        // Generate transform with origin located at the lower left corner
        Transform(float x_pos = 0.0f, float y_pos = 0.0f, float width = 0.0f, float height = 0.0f, float x_origin = 0.0f, float y_origin = 1.0);

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
        double get_rotation() const {return m_angle;}
        void set_rotation_center(float x, float y);
        Point get_rotation_center() const {return{m_x_rotate,m_y_rotate};}

        bool is_rotated() const;
        bool is_scaled() const;

        // Get position at normalized coordinates x and y relative to upper left corner of transform
        // Example: 0.0,0.0 Upper left corner; 0.5,0.5 middle point; 1.0,1.0 lower right corner
        Point get_relative(float x, float y) const;
        Point get_relative_rotated(float x, float y) const;
        Point get_relative_bounding_box(float x, float y) const;

        Dimensions get_base_dimensions() const {return {m_width, m_height};}
        Dimensions get_dimensions() const {return{m_width * m_x_scale, m_height * m_y_scale};}
        Scale get_scale() const {return{m_x_scale,m_y_scale};}

        // Generate rect with position relative to upper left corner and casted to int
        Rect to_rect() const;
        // In case of rotation compute bounding box
        Rect to_bounding_box() const;

        // Apply transformation to scaled world coordinates
        void transform_hitbox(Rect& hitbox) const;

        // Determine to what the sorting point is relative
        // Only makes a difference if rotation is applied!
        enum SortMode {
            // Based on unrotated rectangle
            base,
            // Sort point moves with rotation
            rotated,
            // Based on relative coords of bounding box
            bounding_box
        };

        void set_sort_mode(SortMode s) {m_sort_mode = s;}
        SortMode get_sort_mode() const {return m_sort_mode;}

        void set_sort_point(float x, float y) {m_x_sort = x;m_y_sort = y;}
        Point get_sort_point() const;

    private:
        float m_x_pos;
        float m_y_pos;
        float m_width;
        float m_height;
        float m_x_scale = 1.0f;
        float m_y_scale = 1.0f;

        double m_angle = 0.0;
        // Origin from which is rotated
        float m_x_rotate = 0.0f;
        float m_y_rotate = 1.0f;

        // Origin from which is scaled
        float m_x_origin = 0.0f;
        float m_y_origin = 1.0f;

        // Relative coordinate used for render sorting
        float m_x_sort = 0.0f;
        float m_y_sort = 1.0f;

        // Default for matching tiled preview independent of rotation point
        SortMode m_sort_mode = SortMode::rotated;

        static const float MIN_SCALE;
        static const float MIN_ROTATION;
};
}} // namespace salmon::internal

#endif // TRANSFORM_HPP_INCLUDED
