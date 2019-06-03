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
#ifndef WIDE_EVENT_COLLECTION_HPP_INCLUDED
#define WIDE_EVENT_COLLECTION_HPP_INCLUDED

#include <map>
#include <string>

#include "event/event_collection.hpp"

template <class Scope>
class SmartEvent;

class WideEventCollection {
    public:
        template<class Scope, class Key=std::string>
        void register_event(std::pair<Key, SmartEvent<Scope>> key_event_pair) {event_map<Scope,Key>().register_event(key_event_pair);}

        template<class Scope, class Key=std::string>
        void register_event(Key name, SmartEvent<Scope> event) {event_map<Scope,Key>().register_event(name, event);}

        template<class Scope, class Key=std::string>
        SmartEvent<Scope> get_event(Key name) const {return event_map<Scope,Key>().get_event(name);}

        template<class Scope, class Key=std::string>
        bool check_event(Key name) const { return event_map<Scope,Key>().check_event(name); }

        template<class Scope, class Key=std::string>
        std::map<Key, SmartEvent<Scope>>& get_container() {return event_map<Scope,Key>().get_container();}

    private:
        template<class Scope, class Key=std::string>
        EventCollection<Scope,Key>& event_map() const { static EventCollection<Scope,Key> events; return events;}
};

#endif // WIDE_EVENT_COLLECTION_HPP_INCLUDED
