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

InputHandler::InputHandler(MapData& mapdata) : m_mapdata{mapdata} {}

/**
 * @brief Links key with event which is sent to player upon press/release
 * @param key The keypress which triggers the event
 * @param event The event which gets triggered by a key
 * @param sustained, up, down Booleans which indicate when the event should be sent
 */
bool InputHandler::register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down) {
    if(!m_mapdata.check_event_convert_actor(event)) {
        std::cerr << "An event called: " << event << " does not exist/ never got parsed!\n";
        return false;
    }
    if( (sustained && up) || (sustained && down) ) {
        std::cerr << "Cant parse key event as sustained AND up or down\n";
        return false;
    }
    else {
        if(sustained) {
            SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
            if(scancode == SDL_SCANCODE_UNKNOWN) {
                std::cerr << "No corresponding scancode to key " << key <<" which is required for checking sustained\n";
                return false;
            }
            SmartEvent<Actor> event_data = m_mapdata.get_event_convert_actor(event);
            SDL_Keysym temp;
            temp.sym = key;
            temp.scancode = scancode;
            event_data->set_cause(Cause(temp));
            m_key_sustained.register_event(scancode, event_data);
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
bool InputHandler::process_key_down(SDL_Event  e) {
    Actor* target = m_mapdata.fetch_actor(m_mapdata.get_key_target());
    if(target != nullptr && m_key_down.check_event(e.key.keysym.sym)) {
        SmartEvent<Actor> event = m_key_down.get_event(e.key.keysym.sym);
        event->set_cause(Cause(e.key.keysym));
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
bool InputHandler::process_key_up(SDL_Event  e) {
    Actor* target = m_mapdata.fetch_actor(m_mapdata.get_key_target());
    if(target != nullptr && m_key_up.check_event(e.key.keysym.sym)) {
        SmartEvent<Actor> event = m_key_up.get_event(e.key.keysym.sym);
        event->set_cause(Cause(e.key.keysym));
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
        for(std::pair<const SDL_Scancode, SmartEvent<Actor>>& x : m_key_sustained.get_container()) {
            if(keys[x.first]) {
                target->get_event_queue().add_event(x.second);
            }
        }
    }
}
