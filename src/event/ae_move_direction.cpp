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
#include "event/ae_move_direction.hpp"

#include <string>

#include "actor/actor.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

std::string AeMoveDirection::m_alias = "AeMoveDirection";

AeMoveDirection::AeMoveDirection(Direction dir, unsigned duration, Priority prio) :
EventContainer(prio, EventSignal::stop),
m_direction{dir},
m_duration{duration}
{

}

/**
 * @brief Move the actor to the supplied direction for x frames
 * @param actor The actor which should move
 * @return @c bool which indicates if the event is fully processed
 */
EventSignal AeMoveDirection::process(Actor& actor) {
    // process stuff
    if (m_duration != 0) {
        std::vector<float> mov_factors = dir_to_mov(m_direction);
        if(actor.move(mov_factors[0], mov_factors[1])) {
            actor.animate(AnimationType::walk, m_direction);
            m_duration--;
        }
        else {m_duration = 0;}
    }
    if (m_duration == 0) return EventSignal::end;
    else return signal();
}

/// Create event and return pointer to it
AeMoveDirection* AeMoveDirection::create(Direction dir, unsigned duration, Priority prio) {
    AeMoveDirection temp(dir, duration, prio);
    return duplicate(temp);
}
