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
#include "core/input_cache.hpp"

using salmon::MouseState;
using salmon::MouseButtonState;

bool InputCache::is_down(SDL_Keycode key) const {return m_keys[SDL_GetScancodeFromKey(key)];}
bool InputCache::just_pressed(SDL_Keycode key) const {return m_pressed.count(key);}
bool InputCache::just_released(SDL_Keycode key) const {return m_released.count(key);}

void InputCache::set_key(SDL_Keycode key, bool pressed) {
    if(pressed) {
        m_pressed.insert(key);
    }
    else {
        m_released.insert(key);
    }
}

void InputCache::poll() {
    m_keys = SDL_GetKeyboardState(NULL);
}

void InputCache::clear() {
     m_pressed.clear();
     m_released.clear();
}
