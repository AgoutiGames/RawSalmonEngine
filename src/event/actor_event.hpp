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
#ifndef ACTOR_EVENT_HPP_INCLUDED
#define ACTOR_EVENT_HPP_INCLUDED

//#include "actor/actor.hpp"
#include "event/base_event.hpp"
#include "util/game_types.hpp"

class Actor;
//forward declare all Actor Events (for initialize_all method)
class AeMoveDirection;

template <class T>

class ActorEvent : public BaseEvent<T>{

    public:
        ActorEvent(Priority prio) : BaseEvent<T>(prio) {}
        virtual bool process(Actor& actor) = 0;
        virtual ~ActorEvent() = 0;

        static void initialize_all();

    private:
};

template <class T> void ActorEvent<T>::initialize_all() {
    BaseEvent<AeMoveDirection>::initialize();
}

template <class T> ActorEvent<T>::~ActorEvent() {}

#endif // ACTOR_EVENT_HPP_INCLUDED
