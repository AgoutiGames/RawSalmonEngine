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
#ifndef EVENT_CONTAINER_HPP_INCLUDED
#define EVENT_CONTAINER_HPP_INCLUDED

#include <list>

#include "util/game_types.hpp"

template <class T, class U>

class EventContainer : public T{
    public:
        EventContainer(Priority prio) : m_priority{prio} {}

        static bool kill(U* event) {
            for(auto it=m_event_list.begin(); it != m_event_list.end(); ++it) {
                if(&*it == event) {
                    m_event_list.erase(it);
                    return true;
                }
            }
            return false;
        }

        virtual Priority priority() {return m_priority;}

        static U* duplicate(U* event);
        static U* duplicate(U& event);
        virtual ~EventContainer() = 0;

        Priority m_priority;

        static void initialize();

    private:
        static std::list<U> m_event_list;
};

template <class T, class U> std::list<U> EventContainer<T, U>::m_event_list;

template <class T, class U> void EventContainer<T, U>::initialize() {
    m_event_list.clear();
}

template <class T, class U> U* EventContainer<T, U>::duplicate(U* event) {
            m_event_list.push_back(U(*event));
            return &m_event_list.back();
        }

template <class T, class U> U* EventContainer<T, U>::duplicate(U& event) {
            m_event_list.push_back(U(event));
            return &m_event_list.back();
        }

template <class T, class U> EventContainer<T, U>::~EventContainer() {}

#endif // EVENT_CONTAINER_HPP_INCLUDED
