/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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

Camera::Camera(int x, int y, int w, int h) :
    m_rect{x,y,w,h} {}

/**
 * @brief Move camera to a valid position in respect to player position and map border
 *
 * @note If the camera is map bound and the map is smaller than the camera, the map is centered inside the camera frame
 */
void Camera::update() {
    if(m_player_bound) {
        m_rect.x = m_player->get_x_center() - (m_rect.w / 2);
        m_rect.y = m_player->get_y_center() - (m_rect.h / 2);
    }
    if(m_map_bound) {
        // Check if map is less wide than the effective camera frame
        if( static_cast<int>(m_map_width) - m_crop_left - m_crop_right < m_rect.w ) {
            // Center the camera horizontally relative to map
            m_rect.x = ((static_cast<int>(m_map_width) - m_crop_left - m_crop_right) - m_rect.w) / 2;
        }
        else {
            // Check horizontal borders
            if(m_rect.x < m_crop_left) {
                m_rect.x = m_crop_left;
            }
            else if(m_rect.x > static_cast<int>(m_map_width) - m_rect.w - m_crop_right) {
                m_rect.x = m_map_width - m_rect.w - m_crop_right;
            }
        }

        // Check if map is less high than the effective camera frame
        if( static_cast<int>(m_map_height) - m_crop_up - m_crop_down < m_rect.h ) {
            // Center the camera vertically relative to map
            m_rect.y = ((static_cast<int>(m_map_height) - m_crop_up - m_crop_down) - m_rect.h) / 2;
        }
        else {
            // Check vertical borders
            if(m_rect.y < m_crop_up) {
                m_rect.y = m_crop_up;
            }
            else if(m_rect.y > static_cast<int>(m_map_height) - m_rect.h - m_crop_down) {
                m_rect.y = m_map_height - m_rect.h - m_crop_down;
            }
        }
    }
}

/**
 * @brief Move camera to the supplied position in respect to map border
 *
 * @note If the camera is player bound, the player position has precedence over the supplied coordinates
 */
void Camera::update(int x, int y) {
    m_rect.x = x;
    m_rect.y = y;
    update();
}

/**
 * @brief Bind the camera center to the player image center
 */
void Camera::bind_player(Actor* actor) {
    m_player = actor;
    m_player_bound = true;
}

/**
 * @brief Bind the camera to the map boundaries so that we can't look over the map borders
 */
void Camera::bind_map(unsigned w, unsigned h) {
    m_map_width = w;
    m_map_height = h;
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
