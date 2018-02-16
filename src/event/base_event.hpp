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
#ifndef BASE_EVENT_HPP_INCLUDED
#define BASE_EVENT_HPP_INCLUDED

#include <vector>

#include "util/game_types.hpp"

template <class T>

class BaseEvent {
    public:
        BaseEvent(Priority prio) : m_priority{prio} {}

        bool kill(T* event) {
            for(unsigned i = 0; i < m_event_list.size(); i++) {
            if(&(m_event_list[i]) == this) {
                m_event_list.erase(m_event_list.begin() + i);
                return true;
                }
            }
            return false;
        }

        static T* duplicate(T* event);
        static T* duplicate(T& event);
        virtual ~BaseEvent() = 0;

        Priority m_priority;

        static void initialize();

    private:
        static std::vector<T> m_event_list;
};

template <class T> std::vector<T> BaseEvent<T>::m_event_list;

template <class T> void BaseEvent<T>::initialize() {
    m_event_list.clear();
}

template <class T> T* BaseEvent<T>::duplicate(T* event) {
            m_event_list.push_back(T(*event));
            return &m_event_list.back();
        }

template <class T> T* BaseEvent<T>::duplicate(T& event) {
            m_event_list.push_back(T(event));
            return &m_event_list.back();
        }

template <class T> BaseEvent<T>::~BaseEvent() {}

#endif // BASE_EVENT_HPP_INCLUDED
