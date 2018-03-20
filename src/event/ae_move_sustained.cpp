/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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
#include "event/ae_move_sustained.hpp"

#include <string>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

std::string AeMoveSustained::m_alias = "AeMoveSustained";

AeMoveSustained::AeMoveSustained(Direction dir, SDL_Keysym key, Priority prio) :
EventContainer(prio, EventSignal::stop),
m_direction{dir}
{
    set_key(key);
}

/**
 * @brief Move the actor to the supplied direction until key is released
 * @param actor The actor which should move
 * @return @c bool which indicates if the event is fully processed
 */
EventSignal AeMoveSustained::process(Actor& actor) {
    // process stuff
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[get_key().scancode]) {
        std::vector<float> mov_factors = dir_to_mov(m_direction);
        if(actor.move(mov_factors[0], mov_factors[1])) {
            actor.animate(AnimationType::walk, m_direction);
        }
        else {
            actor.animate(AnimationType::idle, m_direction);
        }
        return signal();
    }
    else {

        return EventSignal::end;
    }
}

/// Create event and return pointer to it
AeMoveSustained* AeMoveSustained::create(Direction dir, SDL_Keysym key, Priority prio) {
    AeMoveSustained temp(dir, key, prio);
    return duplicate(temp);
}

