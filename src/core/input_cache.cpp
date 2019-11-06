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
    poll_mouse();
    // poll keyboard
    m_keys = SDL_GetKeyboardState(NULL);
}

void InputCache::clear() {
     m_pressed.clear();
     m_released.clear();
     m_mouse = salmon::MouseState();
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
    }
    else {
        button->released = true;
    }
    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

void InputCache::set(SDL_MouseWheelEvent event) {
    int flipped = 1; // Not flipped
    if(event.direction == SDL_MOUSEWHEEL_FLIPPED) {flipped = -1;}
    m_mouse.x_scroll += event.x * flipped;
    m_mouse.y_scroll += event.y * flipped;

    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

void InputCache::set(SDL_MouseMotionEvent event) {
    m_mouse.x_delta += event.xrel;
    m_mouse.y_delta += event.yrel;
    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

void InputCache::poll_mouse() {
    Uint32 buttons = SDL_GetMouseState(&m_mouse.x_pos, &m_mouse.y_pos);
    if(buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {m_mouse.left.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {m_mouse.right.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {m_mouse.middle.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_X1)) {m_mouse.extra1.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_X2)) {m_mouse.extra2.down = true;}
}
