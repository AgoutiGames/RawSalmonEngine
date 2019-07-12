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
#ifndef EVENT_COLLECTION_HPP_INCLUDED
#define EVENT_COLLECTION_HPP_INCLUDED

#include <map>
#include <string>

template <class Scope>
class SmartEvent;

template<class Scope, class Key=std::string>
class EventCollection {
    public:
        void register_event(std::pair<Key, SmartEvent<Scope>> key_event_pair) {m_events[key_event_pair.first] = key_event_pair.second;}
        void register_event(Key name, SmartEvent<Scope> event) {register_event(std::make_pair(name, event));}
        SmartEvent<Scope> get_event(Key name) const {return m_events.at(name);}
        bool check_event(Key name) const { return m_events.find(name) != m_events.end(); }

        std::map<Key, SmartEvent<Scope>>& get_container() {return m_events;}
    private:
        std::map<Key, SmartEvent<Scope>> m_events;
};

#endif // EVENT_COLLECTION_HPP_INCLUDED
