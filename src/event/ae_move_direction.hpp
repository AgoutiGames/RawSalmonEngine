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
#ifndef AE_MOVE_DIRECTION_HPP_INCLUDED
#define AE_MOVE_DIRECTION_HPP_INCLUDED

#include "event/actor_event.hpp"
#include "event/event_container.hpp"

#include <vector>

#include "util/game_types.hpp"

class AeMoveDirection : public EventContainer<ActorEvent, AeMoveDirection>{

    public:
        AeMoveDirection(Direction dir, unsigned duration = 1, Priority prio = Priority::medium);
        virtual bool process(Actor& actor) override;
        virtual ~AeMoveDirection() override;

        using EventContainer::kill;
        void kill() {kill(this);}

        static AeMoveDirection* create(Direction dir, unsigned duration = 1, Priority prio = Priority::medium);

    private:
        Direction m_direction;
        unsigned m_duration; ///< How many frames the movement persists

};

#endif // AE_MOVE_DIRECTION_HPP_INCLUDED
