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

#include "util/logger.hpp"

using salmon::MouseState;
using salmon::MouseButtonState;

bool InputCache::is_down(SDL_Keycode key) const {return m_keys[SDL_GetScancodeFromKey(key)];}
bool InputCache::just_pressed(SDL_Keycode key) const {return m_pressed.count(key);}
bool InputCache::just_released(SDL_Keycode key) const {return m_released.count(key);}

void InputCache::set(SDL_Keycode key, bool pressed) {
    if(pressed) {
        m_pressed.insert(key);
    }
    else {
        m_released.insert(key);
    }
}

void InputCache::poll() {
    // poll keyboard
    m_keys = SDL_GetKeyboardState(NULL);
}

void InputCache::clear() {
     m_pressed.clear();
     m_released.clear();
     //m_mouse = salmon::MouseState();
     // Preserve x_pos, y_pos and down values
     m_mouse.x_delta = 0;
     m_mouse.y_delta = 0;
     m_mouse.x_scroll = 0;
     m_mouse.y_scroll = 0;
     m_mouse.left.pressed = false;
     m_mouse.left.released = false;
     m_mouse.middle.pressed = false;
     m_mouse.middle.released = false;
     m_mouse.right.pressed = false;
     m_mouse.right.released = false;
     m_mouse.extra1.pressed = false;
     m_mouse.extra1.released = false;
     m_mouse.extra2.pressed = false;
     m_mouse.extra2.released = false;
}

void InputCache::set(SDL_MouseButtonEvent event) {
    MouseButtonState* button = nullptr;
    if(event.button == SDL_BUTTON_LEFT) {button = &m_mouse.left;}
    else if(event.button == SDL_BUTTON_RIGHT) {button = &m_mouse.right;}
    else if(event.button == SDL_BUTTON_MIDDLE) {button = &m_mouse.middle;}
    else if(event.button == SDL_BUTTON_X1) {button = &m_mouse.extra1;}
    else if(event.button == SDL_BUTTON_X2) {button = &m_mouse.extra2;}
    else {Logger(Logger::error) << "Strange mouse button error, couldn't recognize mouse button!"; return;} // Strange error should not occur ever

    if(event.state == SDL_PRESSED) {
        button->pressed = true;
        button->down = true;
    }
    else {
        button->released = true;
        button->down = false;
    }
    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

void InputCache::set(SDL_MouseWheelEvent event) {
    int flipped = 1; // Not flipped
    if(event.direction == SDL_MOUSEWHEEL_FLIPPED) {flipped = -1;}
    m_mouse.x_scroll += event.x * flipped;
    m_mouse.y_scroll += event.y * flipped;
}

void InputCache::set(SDL_MouseMotionEvent event) {
    m_mouse.x_delta += event.xrel;
    m_mouse.y_delta += event.yrel;
    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

SDL_Keycode InputCache::convert_key(std::string key) {
    SDL_Keycode temp = SDL_GetKeyFromName(key.c_str());
    if(temp == SDLK_UNKNOWN) {
        Logger(Logger::warning) << "The string " << key << " is no proper keyboard key value!";
    }
    return temp;
}

bool InputCache::is_down(std::string key) const {return is_down(convert_key(key));}
bool InputCache::just_pressed(std::string key) const {return just_pressed(convert_key(key));}
bool InputCache::just_released(std::string key) const {return just_released(convert_key(key));}
