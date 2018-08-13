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
#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include <SDL2/SDL.h>

#include "actor/actor.hpp"

/**
 * @brief An interface for a smart sdl_rect used for camera purposes
 */

class Camera{
    public:
        Camera(int x, int y, int w, int h);

        int x() const {return m_rect.x;}
        int y() const {return m_rect.y;}
        int w() const {return m_rect.w;}
        int h() const {return m_rect.h;}

        void set_size(unsigned w, unsigned h) {m_rect.w = w; m_rect.h = h;}
        void set_crop(int l, int r, int u, int d);
        void bind_player(Actor* actor);
        void unbind_player() {m_player_bound = false;}

        void bind_map(unsigned w, unsigned h);
        void unbind_map() {m_map_bound = false;}

        void update();
        void update(int x, int y);

    private:
        SDL_Rect m_rect = {0,0,0,0};

        unsigned m_map_width = 0;
        unsigned m_map_height = 0;

        int m_crop_left = 0;
        int m_crop_right = 0;
        int m_crop_up = 0;
        int m_crop_down = 0;

        Actor* m_player = nullptr;

        bool m_map_bound = false;
        bool m_player_bound = false;
};

#endif // CAMERA_HPP_INCLUDED
