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
#ifndef INPUT_CACHE_HPP_INCLUDED
#define INPUT_CACHE_HPP_INCLUDED

#include <unordered_set>
#include <SDL.h>

#include "util/game_types.hpp"

class InputCache {
    public:
        bool is_down(SDL_Keycode key) const;
        bool just_pressed(SDL_Keycode key) const;
        bool just_released(SDL_Keycode key) const;

        void set_key(SDL_Keycode key, bool pressed);
        void poll();
        void clear();

        salmon::MouseState get_mouse_state() const {return m_mouse;}
    private:
        std::unordered_set<SDL_Keycode> m_pressed;
        std::unordered_set<SDL_Keycode> m_released;
        const Uint8 * m_keys;
        salmon::MouseState m_mouse;
};


#endif // INPUT_CACHE_HPP_INCLUDED
