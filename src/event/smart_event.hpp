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
#ifndef SMART_EVENT_HPP_INCLUDED
#define SMART_EVENT_HPP_INCLUDED

#include <tinyxml2.h>

#include "util/smart.hpp"

class MapData;

template<class Scope>
class Event;

/**
 * @brief A container for our game events which adheres to RAII and should be used in favour to raw pointers
 */
template<class Scope>
class SmartEvent : public Smart<Event<Scope>> {
public:

    /// Explicitly use all constructors of Smart
    /// @note Except "SmartEvent(Event<Scope>* e)", they'd be used implicitly by default constructors anyway
    using Smart<Event<Scope>>::Smart;

    // The more verbose approach
    // SmartEvent() = default;
    // SmartEvent(Event<Scope>* e) : Smart<Event<Scope>>(e) {}

    // Our handy shortcut constructor slapped ontop our Smart container
    SmartEvent(tinyxml2::XMLElement* source, MapData& base_map) : Smart<Event<Scope>>(Event<Scope>::parse(source, base_map)) {}
};



#endif // SMART_EVENT_HPP_INCLUDED
