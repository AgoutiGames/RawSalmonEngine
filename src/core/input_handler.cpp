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

#include "actor/actor.hpp"
#include "actor/cause.hpp"
#include "event/smart_event.hpp"
#include "map/mapdata.hpp"
#include "map/object_layer.hpp"
#include "util/logger.hpp"

InputHandler::InputHandler(MapData& mapdata) : m_mapdata{mapdata} {}

/**
 * @brief Links key with event which is sent to player upon press/release
 * @param key The keypress which triggers the event
 * @param event The event which gets triggered by a key
 * @param sustained, up, down Booleans which indicate when the event should be sent
 */
bool InputHandler::register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down) {
    if(!m_mapdata.check_event_convert_actor(event)) {
        Logger(Logger::error) << "An event called: " << event << " does not exist/ never got parsed!";
        return false;
    }
    if( (sustained && up) || (sustained && down) ) {
        Logger(Logger::error) << "Cant parse key event as sustained AND up or down";
        return false;
    }
    else {
        if(sustained) {
            SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
            if(scancode == SDL_SCANCODE_UNKNOWN) {
                Logger(Logger::error) << "No corresponding scancode to key " << key <<" which is required for checking sustained";
                return false;
            }
            SmartEvent<Actor> event_data = m_mapdata.get_event_convert_actor(event);
            m_key_sustained.register_event(key, event_data);
        }
        if(up) {
            m_key_up.register_event(key,m_mapdata.get_event_convert_actor(event));
        }
        if(down) {
            m_key_down.register_event(key,m_mapdata.get_event_convert_actor(event));
        }
        return true;
    }
}

/**
 * @brief Adds event to player if key is pressed
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool InputHandler::process_key_down(SDL_KeyboardEvent  e) {
    Actor* target = m_mapdata.fetch_actor(m_mapdata.get_key_target());
    if(target != nullptr && m_key_down.check_event(e.keysym.sym)) {
        SmartEvent<Actor> event = m_key_down.get_event(e.keysym.sym);
        target->get_event_queue().add_event(event);
        return true;
    }
    return false;
}

/**
 * @brief Adds event to player if key is released
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool InputHandler::process_key_up(SDL_KeyboardEvent  e) {
    Actor* target = m_mapdata.fetch_actor(m_mapdata.get_key_target());
    if(target != nullptr && m_key_up.check_event(e.keysym.sym)) {
        SmartEvent<Actor> event = m_key_up.get_event(e.keysym.sym);
        target->get_event_queue().add_event(event);
        return true;
    }
    return false;
}

/**
 * @brief Checks if key is down and sends associated event to player
 */
void InputHandler::process_keys_sustained() {
    Actor* target = m_mapdata.fetch_actor(m_mapdata.get_key_target());
    if(target != nullptr) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        for(std::pair<const SDL_Keycode, SmartEvent<Actor>>& x : m_key_sustained.get_container()) {
            if(keys[SDL_GetScancodeFromKey(x.first)]) {
                target->get_event_queue().add_event(x.second);
            }
        }
    }
}

void InputHandler::prime_mouse_button(SDL_MouseButtonEvent event) {
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

void InputHandler::prime_scroll(SDL_MouseWheelEvent event) {
    int flipped = 1; // Not flipped
    if(event.direction == SDL_MOUSEWHEEL_FLIPPED) {flipped = -1;}
    m_mouse.x_scroll += event.x * flipped;
    m_mouse.y_scroll += event.y * flipped;

    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

void InputHandler::prime_mouse_movement(SDL_MouseMotionEvent event) {
    m_mouse.x_delta += event.xrel;
    m_mouse.y_delta += event.yrel;
    m_mouse.x_pos = event.x;
    m_mouse.y_pos = event.y;
}

/// @note Always also polls the current mouse position
void InputHandler::prime_mouse_pressed() {
    Uint32 buttons = SDL_GetMouseState(&m_mouse.x_pos, &m_mouse.y_pos);
    if(buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {m_mouse.left.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {m_mouse.right.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {m_mouse.middle.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_X1)) {m_mouse.extra1.down = true;}
    if(buttons & SDL_BUTTON(SDL_BUTTON_X2)) {m_mouse.extra2.down = true;}
}

void InputHandler::finalize_mouse_state() {
    SDL_Rect cam = m_mapdata.get_camera().get_rect();
    // Transform cursor from camera space to global space
    SDL_Point click{m_mouse.x_pos + cam.x, m_mouse.y_pos+cam.y};

    // Fetch all currently visible actors
    std::vector<Actor*> actors;
    auto layers = m_mapdata.get_layer_collection().get_object_layers();
    for(ObjectLayer* layer : layers) {
        std::vector<Actor*> temp = layer->get_clip(cam);
        actors.insert(actors.end(),temp.begin(),temp.end());
    }

    // Check all hitboxes of each actor if they intersect with the mouse cursor
    for(Actor* a : actors) {
        for(std::pair<std::string, SDL_Rect> hitbox : a->get_hitboxes()) {
            if(SDL_PointInRect(&click,&hitbox.second)) {
                // Trigger the OnMouse response
                a->respond(Response::on_mouse, Cause(hitbox.first));
            }
        }
    }
}
