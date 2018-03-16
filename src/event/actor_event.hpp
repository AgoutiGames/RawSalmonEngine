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

#include <map>
#include <string>

#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/// Category via which all actor events can be handled
/// @warning Always add new actor events to the initialize_all method!
class ActorEvent{

    public:
        ActorEvent() {}
        virtual EventSignal process(Actor& actor) = 0;
        virtual void kill() = 0;
        virtual Priority priority() = 0;
        virtual void set_priority(Priority x) = 0;
        virtual EventSignal signal() = 0;
        virtual void set_signal(EventSignal x) = 0;
        virtual ~ActorEvent() = 0;
        virtual std::string get_type() = 0;

        static void initialize_all();

        template <class T>
        static void register_class();

    private:

        static std::map<std::string, ActorEvent*> m_event_dict;

};

template <class T>
void ActorEvent::register_class() {
    EventContainer<ActorEvent, T>::initialize();
    m_event_dict[T::get_type_static()] = T::create();
}

#endif // ACTOR_EVENT_HPP_INCLUDED
