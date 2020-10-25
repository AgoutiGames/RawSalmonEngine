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

#include "./types.hpp"

namespace salmon {

// Rectangular area easy to transform
class Transform{
    public:
        /// Generate transform with origin located at the lower left corner
        Transform(float x_pos = 0.0f, float y_pos = 0.0f, float width = 0.0f, float height = 0.0f, float x_origin = 0.0f, float y_origin = 1.0);

        /// Set the position of the transform which means the location of its origin in world coordinates
        void set_pos(float x, float y);
        /// Moves the transform in such a way that the given relative point is at the destination world coordinates
        /// @note Takes rotation into account!
        void set_pos(float rel_src_x, float rel_src_y, float dest_x, float dest_y);
        /// Shift the whole transform in x and y direction
        void move_pos(float x, float y);
        /// Set the base width and height of the transform in pixels
        void set_dimensions(float w, float h);
        /// Set origin using normalized relative coordinates wit origin in upper left corner
        /// @example (0,0) Upper left corner (0.5,0.5) Center (0,1.0) Lower left corner
        /// @note origin is the center of each scale operation
        void set_origin(float x, float y);

        /// Set scale of transform to absolute values
        void set_scale(float x, float y);
        /// Scale relative to current scale
        void scale(float x, float y);

        /// Set the curent rotation in degrees
        /// @note Negative values and values beyond (-)360 are valid
        void set_rotation(double angle) {m_angle = angle;}
        /// Add degrees to current rotation
        void rotate(double angle) {m_angle += angle;}
        /// Read out current rotation in degrees
        double get_rotation() const {return m_angle;}
        /// Set the center of rotation in normalized relative coordinates
        /// @note Preserves old rotation by recalculating the origin
        void set_rotation_center(float x, float y);
        /// Gets the current center of rotation in normalized relative coordinates
        Point get_rotation_center() const {return{m_x_rotate,m_y_rotate};}

        /// Returns true if rotation surpasses a minimum value
        bool is_rotated() const;
        /// Returns true if scaling surpasses a minimum value
        bool is_scaled() const;
        /// Returns true if transform is flipped on any axis
        bool is_flipped() const {return m_vertical_flip || m_horizontal_flip;}

        /// Returns true if the position of the transform (could have) changed since "was_moved" was called the last time
        /// @note Is for safety reasons also always the case after being scaled
        bool was_moved();
        /// Returns true if the dimensions of the transform (could have) changed since "was_scaled" was called the last time
        bool was_scaled();

        /// Get position at normalized coordinates x and y relative to upper left corner of transform
        /// @example 0.0,0.0 Upper left corner; 0.5,0.5 middle point; 1.0,1.0 lower right corner
        Point get_relative(float x, float y) const;
        /// Same as get_relative but with taking rotation into account
        Point get_relative_rotated(float x, float y) const;
        /// Same as get relative but based on the bounding box around the rotated transform
        Point get_relative_bounding_box(float x, float y) const;

        /// Returns the base width and height in pixels
        Dimensions get_base_dimensions() const {return {m_width, m_height};}
        /// Returns the current width and height in pixels
        Dimensions get_dimensions() const {return{m_width * m_x_scale, m_height * m_y_scale};}
        /// Returns the current scaling which modifies the base dimensions
        Scale get_scale() const {return{m_x_scale,m_y_scale};}

        /// Generate rect with position relative to upper left corner
        Rect to_rect() const;
        /// Same as to rect but computes a bounding box instead if rotation is applied
        Rect to_bounding_box() const;

        /**
         * @brief Transform given hitbox to world coordinates based on this transforms state
         * The position of the raw hitbox is in relation to the upper left corner of the transform
         * hence it is necessary to convert it to world coordinates for real calcultations.
         *
         * Additionaly:
         * - The hitbox scales with the transform to always match the region on the texture
         * - Flipping of the transform is also applied to the hitbox
         * - If the rotation matches closely a multiple of (-)90 deg, the hitbox also matches the rotation
         */
        void transform_hitbox(Rect& hitbox) const;

        /// Determines the point value used for sorting renderable entities in normalized relative coordinates
        void set_sort_point(float x, float y) {m_x_sort = x;m_y_sort = y;}
        /// Returns the current sort point in !world coordinates!
        Point get_sort_point() const;

        /// Determine to what the sorting point is relative
        /// Only makes a difference if rotation is applied!
        enum SortMode {
            /// Based on unrotated rectangle
            base,
            /// Sort point moves with rotation
            rotated,
            /// Based on relative coords of bounding box
            bounding_box
        };

        void set_sort_mode(SortMode s) {m_sort_mode = s;}
        SortMode get_sort_mode() const {return m_sort_mode;}

        void set_h_flip(bool val) {m_horizontal_flip = val;}
        bool get_h_flip() const {return m_horizontal_flip;}

        void set_v_flip(bool val) {m_vertical_flip = val;}
        bool get_v_flip() const {return m_vertical_flip;}

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

        bool m_horizontal_flip = false;
        bool m_vertical_flip = false;

        bool m_moved = false;
        bool m_scaled = false;

        static const float MIN_SCALE;
        static const float MIN_ROTATION;
};
} // namespace salmon

#endif // TRANSFORM_HPP_INCLUDED
