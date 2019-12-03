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
#ifndef INPUT_CACHE_REF_HPP_INCLUDED
#define INPUT_CACHE_REF_HPP_INCLUDED

#include <string>

#include "./types.hpp"

class InputCache;

namespace salmon {
    class InputCacheRef {
        public:
            InputCacheRef(InputCache& impl);

            bool is_down(std::string key) const;
            bool just_pressed(std::string key) const;
            bool just_released(std::string key) const;

            salmon::MouseState get_mouse_state() const;
            unsigned get_gamepad_count() const;
            /// @note Starts at zero!
            salmon::GamepadState get_gamepad(int index) const;

            bool rumble(int gamepad_index, float strength, unsigned length_ms) const;
            bool stop_rumble(int gamepad_index) const;
        private:
            InputCache* m_impl;
    };
}


#endif // INPUT_CACHE_REF_HPP_INCLUDED
