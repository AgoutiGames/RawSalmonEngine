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
#ifndef INPUT_HANDLER_HPP_INCLUDED
#define INPUT_HANDLER_HPP_INCLUDED

#include <SDL.h>

#include "event/event_collection.hpp"
#include "util/game_types.hpp"

class Actor;
class MapData;

class InputHandler {
    public:
        InputHandler(MapData& mapdata);

        bool register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down);
        bool process_key_up(SDL_KeyboardEvent e);
        bool process_key_down(SDL_KeyboardEvent e);
        void process_keys_sustained();

        // Fetch all mouse events and information for current frame
        void prime_mouse_button(SDL_MouseButtonEvent event);
        void prime_scroll(SDL_MouseWheelEvent event);
        void prime_mouse_movement(SDL_MouseMotionEvent event);
        void prime_mouse_pressed();

        // Check for collision of actors with mouse and trigger the OnMouse response
        void reset() {m_mouse = MouseState();}
        void finalize_mouse_state();

        MouseState get_mouse_state() const {return m_mouse;}

    private:
        MapData& m_mapdata;

        MouseState m_mouse;
        EventCollection<Actor, SDL_Keycode> m_key_up;
        EventCollection<Actor, SDL_Keycode> m_key_down;
        EventCollection<Actor, SDL_Keycode> m_key_sustained;
};


#endif // INPUT_HANDLER_HPP_INCLUDED
