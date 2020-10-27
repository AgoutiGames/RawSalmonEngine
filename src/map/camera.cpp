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
#include "map/camera.hpp"

#include "actor/actor.hpp"
#include "map/mapdata.hpp"

namespace salmon { namespace internal {

Camera::Camera(Rect r, MapData* map) :
    m_transform{r.x,r.y,r.w,r.h,0,0}, m_map{map} {}

/**
 * @brief Move camera to a valid position in respect to player position and map border
 *
 * @note If the camera is map bound and the map is smaller than the camera, the map is centered inside the camera frame
 */
void Camera::update() {
    if(m_actor_bound) {
        auto center = m_player->get_transform().get_relative(0.5,0.5);
        m_transform.set_pos(0.5,0.5,center.x,center.y);
    }
    if(m_map_bound) {
        Point up_left_corner = m_transform.get_relative(0,0);
        Point lower_right_corner = m_transform.get_relative(1,1);
        // Check if map is less wide than the effective camera frame
        if( static_cast<float>(m_map_width) - m_crop_left - m_crop_right < m_transform.get_dimensions().w ) {
            // Center the camera horizontally relative to map
            Point c_center = m_transform.get_relative(0.5,0.5);
            float map_x_center = ((m_map_width - m_crop_right) / 2) + m_crop_left;
            m_transform.move_pos(map_x_center - c_center.x,0);
        }
        else {
            // Check horizontal borders
            if(up_left_corner.x < m_crop_left) {
                m_transform.move_pos(m_crop_left - up_left_corner.x,0);
            }
            else if(lower_right_corner.x > static_cast<float>(m_map_width)- m_crop_right) {
                m_transform.move_pos((static_cast<float>(m_map_width)- m_crop_right) - lower_right_corner.x,0);
            }
        }

        // Check if map is less high than the effective camera frame
        if( static_cast<float>(m_map_height) - m_crop_up - m_crop_down < m_transform.get_dimensions().h ) {
            // Center the camera vertically relative to map
            Point c_center = m_transform.get_relative(0.5,0.5);
            float map_y_center = ((m_map_height - m_crop_down) / 2) + m_crop_up;
            m_transform.move_pos(0,map_y_center - c_center.y);
        }
        else {
            // Check vertical borders
            if(up_left_corner.y < m_crop_up) {
                m_transform.move_pos(0,m_crop_up - up_left_corner.y);
            }
            else if(lower_right_corner.y > static_cast<float>(m_map_height) - m_crop_down) {
                m_transform.move_pos(0,(static_cast<float>(m_map_height)- m_crop_down) - lower_right_corner.y);
            }
        }
    }
}

/**
 * @brief Bind the camera center to the player image center
 */
void Camera::bind_actor(Actor* actor) {
    m_player = actor;
    m_actor_bound = true;
}

/**
 * @brief Bind the camera to the map boundaries so that we can't look over the map borders
 */
void Camera::bind_map() {
    m_map_width = m_map->get_w();
    m_map_height = m_map->get_h();
    m_map_bound = true;
}

/**
 * @brief Crop the visual boundaries of the map in each direction
 *
 * @note l = left | r = right | u = up | d = down
 * @note Positive values let you see less, negative values let you see more of the map
 */
void Camera::set_crop(int l, int r, int u, int d) {
    m_crop_left = l;
    m_crop_right = r;
    m_crop_up = u;
    m_crop_down = d;
}

}} // namespace salmon::internal
