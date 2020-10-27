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
#include "core/input_cache.hpp"

#include "util/logger.hpp"
#include "core/gameinfo.hpp"
#include "map/mapdata.hpp"

namespace salmon { namespace internal {

InputCache::InputCache(GameInfo* game) : m_game{game} {}

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
     //m_mouse = MouseState();
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

     // Add clearing of gamepads
     for(Gamepad& g : m_controllers) {
        g.clear();
     }
}

void InputCache::set(SDL_MouseButtonEvent event) {
    ButtonState* button = nullptr;
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

void InputCache::set(SDL_WindowEvent event) {
    switch(event.event) {
        case SDL_WINDOWEVENT_ENTER : {
            m_mouse.mouse_focus = true;
            break;
        }
        case SDL_WINDOWEVENT_LEAVE : {
            m_mouse.mouse_focus = false;
            break;
        }
        default : {
            Logger(Logger::error) << "Invalid window event: " << event.type << " passed to input cache!";
        }
    }
}

void InputCache::set(SDL_ControllerDeviceEvent event) {
    switch(event.type) {
        case SDL_CONTROLLERDEVICEADDED : {
            add_controller(event.which);
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED : {
            remove_controller(event.which);
            break;
        }
        case SDL_CONTROLLERDEVICEREMAPPED : {
            // NO IDEA WHAT TO DO HERE
            break;
        }
        default : {
            Logger(Logger::error) << "Invalid controller device event: " << event.type << " passed to input cache!";
        }
    }
}
void InputCache::set(SDL_ControllerAxisEvent event) {
    SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(event.axis);
    Gamepad* pad = get_controller(event.which);
    if(pad == nullptr) {
        Logger(Logger::error) << "Unable to find controller: " << event.which << " for axis movement!";
    }
    else {
        pad->set(axis, event.value);
    }
}
void InputCache::set(SDL_ControllerButtonEvent event) {
    SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(event.button);
    Gamepad* pad = get_controller(event.which);
    if(pad == nullptr) {
        Logger(Logger::error) << "Unable to find controller: " << event.which << " for button press!";
    }
    else {
        bool down;
        if(event.state == SDL_PRESSED) {down = true;}
        else {down = false;}
        pad->set(button, down);
    }
}

void InputCache::set(SDL_TouchFingerEvent event) {
    if(m_emulate_touch_as_mouse) {
        switch(event.type) {
            case SDL_FINGERDOWN : {
                if(m_current_finger == 0) {
                    m_current_finger = event.fingerId;
                    m_mouse.left.pressed = true;
                    m_mouse.left.down = true;
                }
                break;
            }
            case SDL_FINGERUP : {
                if(m_current_finger == event.fingerId) {
                    m_current_finger = 0;
                    m_mouse.left.down = false;
                    m_mouse.left.released = true;
                }
                break;
            }
            case SDL_FINGERMOTION : {
                if(m_current_finger == event.fingerId) {
                    PixelDimensions d = m_game->get_map().get_camera().get_transform().get_dimensions();
                    m_mouse.x_delta = static_cast<int>(event.dx * d.w);
                    m_mouse.y_delta = static_cast<int>(event.dy * d.h);
                }
                break;
            }
            default : {
                Logger(Logger::error) << "Invalid touch finger event: " << event.type << " passed to input cache!";
                return;
            }
        }
        if(m_current_finger == event.fingerId) {
            PixelDimensions d = m_game->get_map().get_camera().get_transform().get_dimensions();
            m_mouse.x_pos = static_cast<int>(event.x * d.w);
            m_mouse.y_pos = static_cast<int>(event.y * d.h);
        }
    }
    else {
        // Here should go the proper touch implementation
    }
}

/*
void InputCache::init_controllers() {
    int controller_count = SDL_NumJoysticks();
    for(int i = 0; i < controller_count; i++) {
        add_controller(i);
    }
}*/

bool InputCache::add_controller(int joystick_device_index) {
    if(SDL_IsGameController(joystick_device_index)) {
        SDL_GameController* cont = SDL_GameControllerOpen(joystick_device_index);
        if(cont != nullptr) {
            m_controllers.emplace_back(cont);
            Logger() << "Properly recognized game controller: " << SDL_GameControllerName(cont);
            return true;
        }
        else {
            Logger(Logger::warning) << "The controller called: " << SDL_JoystickNameForIndex(joystick_device_index) << " can't be recognized!";
        }
    }
    else {
        Logger(Logger::warning) << "The controller called: " << SDL_JoystickNameForIndex(joystick_device_index) << " can't be recognized!";
    }
    return false;
}

bool InputCache::remove_controller(SDL_JoystickID instance_id) {
    for(size_t i = 0; i < m_controllers.size(); i++) {
        if(m_controllers[i].get_id() == instance_id) {
            Logger() << "Removed game controller: " << m_controllers[i].get_name();
            m_controllers[i].close();
            m_controllers.erase(m_controllers.begin()+i);
            return true;
        }
    }
    return false;
}

Gamepad* InputCache::get_controller(SDL_JoystickID instance_id) {
    for(size_t i = 0; i < m_controllers.size(); i++) {
        if(m_controllers.at(i).get_id() == instance_id) {
            return &m_controllers.at(i);
        }
    }
    return nullptr;
}

bool InputCache::rumble(int gamepad_index, float strength, unsigned length_ms) const {
    return m_controllers.at(gamepad_index).rumble(strength,length_ms);
}
bool InputCache::stop_rumble(int gamepad_index) const {
    return m_controllers.at(gamepad_index).stop_rumble();
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

}} // namespace salmon::internal
