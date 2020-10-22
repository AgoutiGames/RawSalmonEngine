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
#ifndef GAMEPAD_HPP_INCLUDED
#define GAMEPAD_HPP_INCLUDED

#include <SDL.h>
#include "util/game_types.hpp"

namespace salmon { namespace internal {

class Gamepad {
    public:
        Gamepad(SDL_GameController* controller);

        void set(SDL_GameControllerButton button, bool down);
        void set(SDL_GameControllerAxis axis, Sint16 value);

        bool rumble(float strength, unsigned length_ms) const;
        bool stop_rumble() const;

        SDL_JoystickID get_id() const {return m_id;}
        const char* get_name() const;
        GamepadState get_state() const {return m_state;}

        void close(); // close gamepad device
        void clear(); // Clear old input
    private:
        SDL_GameController* m_controller;
        SDL_JoystickID m_id;
        GamepadState m_state;

        SDL_Haptic* m_haptic = nullptr;
};
}} // namespace salmon::internal

#endif // GAMEPAD_HPP_INCLUDED
