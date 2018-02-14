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
 #include "event/base_event.hpp"

template <class T>
bool BaseEvent<T>::kill() {
    for(unsigned i = 0; i < T::m_event_list; i++) {
        if(&(T::m_event_list[i]) == this) {
            T::m_event_list.erase(i);
            return true;
        }
    }
    return false;
 }

 template <class T>
 T* BaseEvent<T>::duplicate() {
    T::m_event_list.push(T(this));
    return &(T::m_event_list.at(T::m_event_list.size() - 1));
 }
