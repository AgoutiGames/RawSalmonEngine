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
#include <string>

#include "util/game_types.hpp"

/**
 * @brief Manage all constructed events of supplied type U to prevent memory leaks
 * @note This class also enables the inheritance from a base class category to manage
 *       the events via a pointer of this type
 *       class T -> event category
 *       class U -> actual event type
 */

template <class T, class U>

class EventContainer : public T{
    public:
        EventContainer(Priority prio = Priority::medium, EventSignal signal = EventSignal::next, std::string name = "")
        : m_priority{prio}, m_signal{signal}, m_name{name} {}

        /**
         * @brief Deletes the data of the event in the @c event_list
         * @param event Pointer to the event
         * @return @c bool which indicates if the event data could be found
         */
        static bool kill(U* event) {
            for(auto it=m_event_list.begin(); it != m_event_list.end(); ++it) {
                if(&*it == event) {
                    m_event_list.erase(it);
                    return true;
                }
            }
            return false;
        }

        /// Returns the priority value of the event
        virtual Priority priority() const override {return m_priority;}

        /// Sets the priority value of the event
        virtual void set_priority(Priority x) override {m_priority = x;}

        /// Returns the default signal of the event
        virtual EventSignal signal() const override {return m_signal;}

        /// Sets the signal value of the event
        virtual void set_signal(EventSignal x) override {m_signal = x;}

        /// Returns the default name of the event
        virtual std::string name() const override {return m_name;}

        /// Sets the name value of the event
        virtual void set_name(std::string x) override {m_name = x;}

        /**
         * @brief duplicates given event
         * @param event The event which should be duplicated
         * @return Pointer to the duplicate of the event
         */
        static U* duplicate(const U* event);

        /**
         * @brief duplicates given event
         * @param event The event which should be duplicated
         * @return Pointer to the duplicate of the event
         */
        static U* duplicate(const U& event);


        virtual ~EventContainer() = 0;

        /// Deletes the @c event_list variable
        static void initialize();

    private:
        Priority m_priority;
        EventSignal m_signal;
        std::string m_name;

    public:
        static std::list<U> m_event_list;
};

template <class T, class U> std::list<U> EventContainer<T, U>::m_event_list;

template <class T, class U> void EventContainer<T, U>::initialize() {
    m_event_list.clear();
}

template <class T, class U> U* EventContainer<T, U>::duplicate(const U* event) {
            m_event_list.push_back(U(*event));
            return &m_event_list.back();
        }

template <class T, class U> U* EventContainer<T, U>::duplicate(const U& event) {
            m_event_list.push_back(U(event));
            return &m_event_list.back();
        }

template <class T, class U> EventContainer<T, U>::~EventContainer() {}

#endif // EVENT_CONTAINER_HPP_INCLUDED
