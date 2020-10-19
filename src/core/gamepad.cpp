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
#include "core/gamepad.hpp"

#include "util/logger.hpp"

using namespace salmon::internal;

Gamepad::Gamepad(SDL_GameController* controller) : m_controller{controller},
m_id{SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controller))},
m_haptic{SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(m_controller))} {
    if(m_haptic != nullptr) {
        if(SDL_HapticRumbleSupported(m_haptic) != SDL_TRUE) {
            m_haptic = nullptr;
        }
        else if(SDL_HapticRumbleInit(m_haptic) != 0) {
            m_haptic = nullptr;
        }
    }
}

void Gamepad::close() {
    SDL_GameControllerClose(m_controller);
    if(m_haptic != nullptr) {
        SDL_HapticClose(m_haptic);
    }
}

const char* Gamepad::get_name() const {
    return SDL_GameControllerName(m_controller);
}

void Gamepad::set(SDL_GameControllerButton button, bool down) {
    ButtonState* dest = nullptr;
    switch(button) {
        case SDL_CONTROLLER_BUTTON_A : {
            dest = &m_state.button.a;
            break;
        }
        case SDL_CONTROLLER_BUTTON_B : {
            dest = &m_state.button.b;
            break;
        }
        case SDL_CONTROLLER_BUTTON_X : {
            dest = &m_state.button.x;
            break;
        }
        case SDL_CONTROLLER_BUTTON_Y : {
            dest = &m_state.button.y;
            break;
        }
        case SDL_CONTROLLER_BUTTON_BACK : {
            dest = &m_state.button.back;
            break;
        }
        case SDL_CONTROLLER_BUTTON_GUIDE : {
            dest = &m_state.button.guide;
            break;
        }
        case SDL_CONTROLLER_BUTTON_START : {
            dest = &m_state.button.start;
            break;
        }
        case SDL_CONTROLLER_BUTTON_LEFTSTICK : {
            dest = &m_state.button.left_stick;
            break;
        }
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK : {
            dest = &m_state.button.right_stick;
            break;
        }
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER : {
            dest = &m_state.button.left_shoulder;
            break;
        }
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : {
            dest = &m_state.button.right_shoulder;
            break;
        }
        case SDL_CONTROLLER_BUTTON_DPAD_UP : {
            dest = &m_state.button.up;
            break;
        }
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN : {
            dest = &m_state.button.down;
            break;
        }
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT : {
            dest = &m_state.button.left;
            break;
        }
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT : {
            dest = &m_state.button.right;
            break;
        }
        default : {break;}
    }
    if(dest == nullptr) {
        Logger(Logger::error) << "Unrecognized gamepad button: " << button << " pressed";
    }
    else {
        if(down) {
            dest->pressed = true;
            dest->down = true;
        }
        else {
            dest->released = true;
            dest->down = false;
        }
    }
}

void Gamepad::set(SDL_GameControllerAxis axis, Sint16 value) {
    float* dest = nullptr;
    switch(axis) {
        case SDL_CONTROLLER_AXIS_LEFTX : {
            dest = &m_state.axis.left_x;
            break;
        }
        case SDL_CONTROLLER_AXIS_LEFTY : {
            dest = &m_state.axis.left_y;
            break;
        }
        case SDL_CONTROLLER_AXIS_RIGHTX : {
            dest = &m_state.axis.right_x;
            break;
        }
        case SDL_CONTROLLER_AXIS_RIGHTY : {
            dest = &m_state.axis.right_y;
            break;
        }
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT : {
            dest = &m_state.axis.trigger_left;
            break;
        }
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT : {
            dest = &m_state.axis.trigger_right;
            break;
        }
        default : {break;}
    }
    if(dest == nullptr) {
        Logger(Logger::error) << "Unrecognized gamepad axis: " << axis << " moved";
    }
    *dest = static_cast<float>(value) / SDL_JOYSTICK_AXIS_MAX; // We assume that the abs() of min and max is equal
}

void Gamepad::clear() {
    m_state.button.a.pressed = false;
    m_state.button.b.pressed = false;
    m_state.button.x.pressed = false;
    m_state.button.y.pressed = false;
    m_state.button.back.pressed = false;
    m_state.button.start.pressed = false;
    m_state.button.guide.pressed = false;
    m_state.button.up.pressed = false;
    m_state.button.down.pressed = false;
    m_state.button.left.pressed = false;
    m_state.button.right.pressed = false;
    m_state.button.left_stick.pressed = false;
    m_state.button.right_stick.pressed = false;
    m_state.button.left_shoulder.pressed = false;
    m_state.button.right_shoulder.pressed = false;

    m_state.button.a.released = false;
    m_state.button.b.released = false;
    m_state.button.x.released = false;
    m_state.button.y.released = false;
    m_state.button.back.released = false;
    m_state.button.start.released = false;
    m_state.button.guide.released = false;
    m_state.button.up.released = false;
    m_state.button.down.released = false;
    m_state.button.left.released = false;
    m_state.button.right.released = false;
    m_state.button.left_stick.released = false;
    m_state.button.right_stick.released = false;
    m_state.button.left_shoulder.released = false;
    m_state.button.right_shoulder.released = false;
}

bool Gamepad::rumble(float strength, unsigned length_ms) const {
    if(m_haptic == nullptr) {return false;}
    if(SDL_HapticRumblePlay(m_haptic, strength, length_ms) != 0) {
        return false;
    }
    return true;
}

bool Gamepad::stop_rumble() const {
    if(m_haptic == nullptr) {return false;}
    if(SDL_HapticRumbleStop(m_haptic) != 0) {
        return false;
    }
    return true;
}
