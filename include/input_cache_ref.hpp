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
#ifndef INPUT_CACHE_REF_HPP_INCLUDED
#define INPUT_CACHE_REF_HPP_INCLUDED

#include <string>

#include "./types.hpp"

namespace salmon {

namespace internal{class InputCache;}

class InputCacheRef {
    public:
        InputCacheRef(internal::InputCache& impl);

        /**
         * @note Key names are usually the character produced by pressing the key
         *       Special key names are founde here: https://wiki.libsdl.org/SDL_Keycode
         *
         * @note Common key names: Return, Backspace, Escape, Space, Tab, CapsLock,
         *       Delete, Insert, PageUp, PageDown, Up, Down, Left, Right, F1, F2, F...
         *       Left Alt, Left Ctrl, Left Shift, Right Alt, Right Ctrl, Right Shift,
         *       Keypad 0, Keypad 1, ..., Keypad Enter, Keypad +, Keypad *, ...
         */

        /// Returns true if specified key is currently held down
        bool is_down(std::string key) const;
        /// Returns true if the specified key was up last frame and is down now
        bool just_pressed(std::string key) const;
        /// Returns true if the specified key was down last frame and is up now
        bool just_released(std::string key) const;

        /// Returns MouseState object representing mouse with all buttons and axis
        MouseState get_mouse_state() const;
        /// Returns the count of currently connected and recognized gamepads
        unsigned get_gamepad_count() const;

        /**
         * @brief Returns GamepadState object representing all gamepad buttons and axis
         * @param index The index of chosen gamepad starting at zero
         */
        GamepadState get_gamepad(int index) const;

        /**
         * @brief Triggers rumble function of gamepad
         * @param index The index of chosen gamepad starting at zero
         * @param strength The strength of rumble ranging from 0.0 to 1.0
         * @param length_ms The length of the rumble in milliseconds
         * @return True if rumble works with the gamepad, false if not
         */
        bool rumble(int gamepad_index, float strength, unsigned length_ms) const;
        /// Stops potentially running rumble on supplied gamepad index
        bool stop_rumble(int gamepad_index) const;
    private:
        internal::InputCache* m_impl;
};
}


#endif // INPUT_CACHE_REF_HPP_INCLUDED
