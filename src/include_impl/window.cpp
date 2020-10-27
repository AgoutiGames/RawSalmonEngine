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
#include "window.hpp"

#include <SDL.h>

#include "util/logger.hpp"
#include "util/game_types.hpp"

namespace salmon {

SDL_Window* Window::init() {
    PixelDimensions wd = m_transform.get_dimensions();
    // As a default generate a centered window with a generic title which isn't maximized but shown
    // Width an height should be set beforehand throug get_transform.set_dimensions()
    m_window = SDL_CreateWindow("Default Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wd.w, wd.h, SDL_WINDOW_SHOWN);
    if(m_window == nullptr) {return nullptr;}
    int x,y;
    SDL_GetWindowPosition(m_window,&x,&y);
    m_transform.set_pos(0,0,x,y);
    return m_window;
}

void Window::sync_pos() {
    int x,y;
    SDL_GetWindowPosition(m_window,&x,&y);
    m_transform.set_pos(0,0,x,y);

    // Clear moved state from transform
    m_transform.was_moved();
}

void Window::update() {
    // Firstly apply possible changes to transform to the window
    if(m_transform.was_scaled()) {
        PixelRect r = m_transform.to_rect();
        SDL_SetWindowSize(m_window,r.w,r.h);
        SDL_SetWindowPosition(m_window,r.x,r.y);
    }
    else if(m_transform.was_moved()) {
        PixelRect r = m_transform.to_rect();
        SDL_SetWindowPosition(m_window,r.x,r.y);
    }

    // Secondly mirror the window state to the transform
    // Take scale into account but not preserve it
    // Preserve origin of transform!
    int x,y,w,h;
    SDL_GetWindowPosition(m_window,&x,&y);
    SDL_GetWindowSize(m_window,&w,&h);
    m_transform.set_scale(1,1);
    m_transform.set_dimensions(w,h);
    m_transform.set_pos(0,0,x,y);

    // Clear moved and scaled state from transform
    m_transform.was_moved();
    m_transform.was_scaled();
}

PixelRect Window::get_screen_size() const {
    int display_index = SDL_GetWindowDisplayIndex(m_window);
    if(display_index < 0) {
        internal::Logger(internal::Logger::error) << "Cant query current display index, SDL Error: " << SDL_GetError();
        return {};
    }
    SDL_Rect r;
    SDL_GetDisplayBounds(display_index,&r);
    return internal::make_rect(r);
}

PixelRect Window::get_usable_screen_size() const {
    int display_index = SDL_GetWindowDisplayIndex(m_window);
    if(display_index < 0) {
        internal::Logger(internal::Logger::error) << "Cant query current display index, SDL Error: " << SDL_GetError();
        return {};
    }
    SDL_Rect r;
    SDL_GetDisplayUsableBounds(display_index,&r);
    return internal::make_rect(r);
}

std::string Window::get_display_name() const {
    int display_index = SDL_GetWindowDisplayIndex(m_window);
    if(display_index < 0) {
        internal::Logger(internal::Logger::error) << "Cant query current display index, SDL Error: " << SDL_GetError();
        return {};
    }
    return SDL_GetDisplayName(display_index);
}

void Window::set_title(std::string title) {
    SDL_SetWindowTitle(m_window,title.c_str());
}
std::string Window::get_title() const {
    return SDL_GetWindowTitle(m_window);
}

bool Window::set_fullscreen(bool mode) {
    Uint32 flags;
    // Use "fake" fullscreen because its much less error prone on linux and windows
    if(mode) {flags = SDL_WINDOW_FULLSCREEN_DESKTOP;}
    else {flags = 0;}
    if(SDL_SetWindowFullscreen(m_window,flags)) {
        if(mode) {
            internal::Logger(internal::Logger::error) << "Failed to set window to fullscreen, SDL Error: " << SDL_GetError();
        }
        else {
            internal::Logger(internal::Logger::error) << "Failed to set windowed mode, SDL Error: " << SDL_GetError();
        }
        return false;
    }
    return true;
}
bool Window::is_fullscreen() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_FULLSCREEN_DESKTOP;
}

void Window::set_resizable(bool mode) {
    if(mode) {SDL_SetWindowResizable(m_window, SDL_TRUE);}
    else {SDL_SetWindowResizable(m_window, SDL_FALSE);}
}
bool Window::is_resizable() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_RESIZABLE;
}

void Window::set_borderless(bool mode) {
    if(!mode) {SDL_SetWindowBordered(m_window,SDL_TRUE);}
    else {SDL_SetWindowBordered(m_window,SDL_FALSE);}
}
bool Window::is_borderless() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_BORDERLESS;
}

void Window::grab_input(bool mode) {
    if(mode) {SDL_SetWindowGrab(m_window,SDL_TRUE);}
    else {SDL_SetWindowGrab(m_window,SDL_FALSE);}
}
bool Window::input_grabbed() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_INPUT_GRABBED;
}

void Window::show() {
    SDL_ShowWindow(m_window);
}
bool Window::is_shown() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_SHOWN;
}

void Window::hide() {
    SDL_HideWindow(m_window);
}
bool Window::is_hidden() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_HIDDEN;
}

void Window::minimize() {
    SDL_MinimizeWindow(m_window);
}
bool Window::is_minimized() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_MINIMIZED;
}

void Window::maximize() {
    SDL_MaximizeWindow(m_window);
}
bool Window::is_maximized() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_MAXIMIZED;
}

void Window::raise() {
    SDL_RaiseWindow(m_window);
}
bool Window::set_active() {
    return SDL_SetWindowInputFocus(m_window);
}
bool Window::is_active() const {
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return flags & SDL_WINDOW_INPUT_FOCUS;
}

void Window::restore() {
    SDL_RestoreWindow(m_window);
}

void Window::destroy() {
    if(m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

} // namespace salmon

