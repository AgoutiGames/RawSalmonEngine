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
#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "types.hpp"

namespace salmon { namespace internal {

class Actor;
class MapData;

/**
 * @brief An interface for a smart rect used for camera purposes
 */

class Camera{
    public:
        Camera(float x, float y, float w, float h, MapData* map);

        float x() const {return m_rect.x;}
        float y() const {return m_rect.y;}
        float w() const {return m_rect.w;}
        float h() const {return m_rect.h;}
        const Rect& get_rect() const {return m_rect;}

        void set_size(float w, float h) {m_rect.w = w; m_rect.h = h;}
        void set_x(float x) {m_rect.x = x;}
        void set_y(float y) {m_rect.y = y;}
        void set_crop(int l, int r, int u, int d);
        void bind_actor(Actor* actor);
        void unbind_actor() {m_actor_bound = false;}

        void bind_map();
        void unbind_map() {m_map_bound = false;}

        void update();
        void update(float x, float y);

    private:
        Rect m_rect = {0,0,0,0};

        unsigned m_map_width = 0;
        unsigned m_map_height = 0;

        int m_crop_left = 0;
        int m_crop_right = 0;
        int m_crop_up = 0;
        int m_crop_down = 0;

        MapData* m_map = nullptr;
        Actor* m_player = nullptr;

        bool m_map_bound = false;
        bool m_actor_bound = false;
};
}} // namespace salmon::internal

#endif // CAMERA_HPP_INCLUDED
