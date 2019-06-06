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
#ifndef SMART_EVENT_HPP_INCLUDED
#define SMART_EVENT_HPP_INCLUDED

#include "util/tinyxml2.h"

class MapData;

template<class Scope>
class Event;

/**
 * @brief A container for our game events which adheres to RAII and should be used in favour to raw pointers
 */
template<class Scope>
class SmartEvent {
public:
    SmartEvent() : m_internal{nullptr} {}
    /// Constructor taking ownership of event
    SmartEvent(Event<Scope>* event) : m_internal{event} {}
    SmartEvent(tinyxml2::XMLElement* source, MapData& base_map) : m_internal{Event<Scope>::parse(source, base_map)} {}
    ~SmartEvent() {purge();}

    SmartEvent(const SmartEvent& other) : m_internal{other.valid() ? other->clone() : nullptr} {}
    SmartEvent& operator=(const SmartEvent& other);

    SmartEvent(SmartEvent&& other) : m_internal{other.m_internal} {other.m_internal = nullptr;}
    SmartEvent& operator=(SmartEvent&& other);

    Event<Scope>* operator->() const {return m_internal;}
    Event<Scope>& operator*() const {return *m_internal;}

    bool valid() const {return (m_internal == nullptr) ? false : true;}
    explicit operator bool() const {return valid();}

private:
    void purge() {if(valid()) {delete m_internal;}}

    Event<Scope>* m_internal;
};

template<class Scope>
SmartEvent<Scope>& SmartEvent<Scope>::operator=(const SmartEvent<Scope>& other) {
    if(this != &other) {
        purge();
        m_internal = other.valid() ? other->clone() : nullptr;
    }
    return *this;
}

template<class Scope>
SmartEvent<Scope>& SmartEvent<Scope>::operator=(SmartEvent<Scope>&& other) {
    if(this != &other) {
        purge();
        m_internal = other.m_internal;
        other.m_internal = nullptr;
    }
    return *this;
}


#endif // SMART_EVENT_HPP_INCLUDED
