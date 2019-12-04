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

#include <string>
#include <unordered_set>
#include <SDL.h>

#include "core/gamepad.hpp"
#include "util/game_types.hpp"

class GameInfo;

class InputCache {
    public:
        InputCache(GameInfo* game);
        // Although noted otherwise not needed
        //void init_controllers();

        bool is_down(std::string key) const;
        bool just_pressed(std::string key) const;
        bool just_released(std::string key) const;

        bool is_down(SDL_Keycode key) const;
        bool just_pressed(SDL_Keycode key) const;
        bool just_released(SDL_Keycode key) const;

        void set(SDL_Keycode key, bool pressed);
        void set(SDL_MouseButtonEvent event);
        void set(SDL_MouseWheelEvent event);
        void set(SDL_MouseMotionEvent event);
        void set(SDL_WindowEvent event);
        void set(SDL_ControllerDeviceEvent event);
        void set(SDL_ControllerAxisEvent event);
        void set(SDL_ControllerButtonEvent event);
        void set(SDL_TouchFingerEvent event);

        void poll();
        void clear();

        unsigned get_gamepad_count() const {return m_controllers.size();}
        salmon::GamepadState get_gamepad(int index) const {return m_controllers.at(index).get_state();}
        bool rumble(int gamepad_index, float strength, unsigned length_ms) const;
        bool stop_rumble(int gamepad_index) const;

        salmon::MouseState get_mouse_state() const {return m_mouse;}

        static SDL_Keycode convert_key(std::string key);
    private:
        GameInfo* m_game;

        std::unordered_set<SDL_Keycode> m_pressed;
        std::unordered_set<SDL_Keycode> m_released;
        const Uint8 * m_keys;
        salmon::MouseState m_mouse;

        // Only track the first finger with a down event as mouse
        // Mixing touch and mouse should get really weird results!
        SDL_FingerID m_current_finger = 0;
        bool m_emulate_touch_as_mouse = true;

        bool add_controller(int joystick_device_index);
        bool remove_controller(SDL_JoystickID instance_id);
        Gamepad* get_controller(SDL_JoystickID instance_id);

        std::vector<Gamepad> m_controllers;
};


#endif // INPUT_CACHE_HPP_INCLUDED
