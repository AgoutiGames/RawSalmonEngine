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

#include "actor/cause.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;
class MapData;

/// Category via which all actor events can be handled
/// @warning Always add new actor events to the initialize_all method!
class ActorEvent{

    public:
        ActorEvent() {}
        virtual EventSignal process(Actor& actor) = 0;
        virtual void kill() = 0;
        virtual Priority priority() const = 0;
        virtual void set_priority(Priority x) = 0;
        virtual EventSignal signal() const = 0;
        virtual void set_signal(EventSignal x) = 0;
        virtual std::string name() const = 0;
        virtual void set_name(std::string x) = 0;
        virtual ~ActorEvent() = 0;
        virtual std::string get_type() const = 0;
        virtual ActorEvent* copy() const = 0;
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const = 0;

        virtual void set_cause(Cause x) {m_cause = x;}
        const Cause& get_cause() const {return m_cause;}
        virtual void set_key(SDL_Keysym x) {m_cause = Cause(x);}
        const SDL_Keysym get_key() const {return m_cause.get_key();}

        static void initialize_all();

        template <class T>
        static void register_class();

        static tinyxml2::XMLError parse_multi(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry);

    private:
        Cause m_cause;

        static std::map<std::string, ActorEvent*> m_event_dict;

};

/// Registers the type of event as an actor event
template <class T>
void ActorEvent::register_class() {
    EventContainer<ActorEvent, T>::initialize();
    m_event_dict[T::get_type_static()] = T::create();
}

#endif // ACTOR_EVENT_HPP_INCLUDED
