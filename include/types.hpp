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
#ifndef SALMON_TYPES_HPP_INCLUDED
#define SALMON_TYPES_HPP_INCLUDED

namespace salmon {
    const std::string DEFAULT_HITBOX = "DEFAULT";

    enum class Direction {
        up = 0,
        up_right = 45,
        right = 90,
        down_right = 135,
        down = 180,
        down_left = 225,
        left = 270,
        up_left = 315,
        current = -2, ///< Keyword for last used direction
        none = -1,
        invalid = -3,
    };

    enum class AnimationType {
        idle,
        walk,
        run,
        jump,
        fall,
        die,
        custom1,
        custom2,
        custom3,
        custom4,
        custom5,
        custom6,
        custom7,
        custom8,
        custom9,
        current, ///< Keyord for last used AnimationType
        none,
        invalid,
    };

    enum class AnimSignal{
        missing = -1,
        none = 0,
        next = 1,
        trigger = 2,
        wrap = 3,
    };

    struct ButtonState {
        bool pressed = false;
        bool released = false;
        bool down = false;
    };

    struct MouseState {
        ButtonState left;
        ButtonState right;
        ButtonState middle;
        ButtonState extra1;
        ButtonState extra2;
        int x_scroll = 0;
        int y_scroll = 0;
        int x_pos = 0;
        int y_pos = 0;
        int x_delta = 0;
        int y_delta = 0;
        bool mouse_focus = false;
    };

    struct GamepadState {
        struct {
        ButtonState a;
        ButtonState b;
        ButtonState x;
        ButtonState y;
        ButtonState back;
        ButtonState guide; /// Many gamepads don't have this one
        ButtonState start;
        ButtonState left_stick;
        ButtonState right_stick;
        ButtonState left_shoulder;
        ButtonState right_shoulder;
        ButtonState up;
        ButtonState down;
        ButtonState left;
        ButtonState right;
        } button;

        struct {
        float left_x = 0; /// Range from -1 to 1
        float left_y = 0; /// Range from -1 to 1
        float right_x = 0; /// Range from -1 to 1
        float right_y = 0; /// Range from -1 to 1
        float trigger_left = 0; /// Range from 0 to 1
        float trigger_right = 0; /// Range from 0 to 1
        } axis;
    };
}

#endif // SALMON_TYPES_HPP_INCLUDED
