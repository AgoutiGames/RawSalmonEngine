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
#ifndef EVENT_QUEUE_HPP_INCLUDED
#define EVENT_QUEUE_HPP_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL.h>

#include "event/smart_event.hpp"
#include "util/game_types.hpp"

/**
 * @brief A field of events which can be processed and sorted according to priorities
 */
template<class Scope>
class EventQueue {
    public:
        EventSignal process_events(Scope& target);

        void add_event(SmartEvent<Scope> event) {m_event_buffer.push_back(event);}
        void add_event_internal(SmartEvent<Scope> event);
        void release_buffer();
        unsigned scrap_event(std::string name, SmartEvent<Scope>* except = nullptr);

        void set_cooldown(std::string name, float dur_sec) {m_timestamp[name] = SDL_GetTicks() + dur_sec * 1000;}
        Uint32 get_cooldown(std::string name) const {return m_timestamp.at(name);}

        void block_event(std::string name) {m_block[name] = true;}
        void block_event(SDL_Keycode key) {m_block_key[key] = true;}

        void unblock_event(std::string name) {m_block[name] = false;}
        void unblock_event(SDL_Keycode key) {m_block_key[key] = false;}

        bool is_empty() const {return m_event_pipeline.empty();}
        int get_size() const {return m_event_pipeline.size();}

        bool is_blocked(std::string name) const;
        bool is_blocked(const SDL_Keysym& key) const;
        bool in_cooldown(std::string name) const;

        std::vector<SmartEvent<Scope>>& get_events() {return m_event_pipeline;}

    private:
        std::map<std::string, Uint32> m_timestamp; ///< Map holding timestamps for use as cooldown functionality
        std::map<std::string, bool> m_block; ///< Map determinig if the event pipeline is blocked for a specific event or event type
        std::map<SDL_Keycode, bool> m_block_key; ///< Map determinig if the event pipeline is blocked for a specific key
        std::vector<SmartEvent<Scope>> m_event_pipeline; ///< Vector of current events to be processed

        std::vector<SmartEvent<Scope>> m_event_buffer; ///< Added events are buffered and released right before processing

};

/**
 * @brief Process the event queue
 * @return a @c Event which indicates if the target should be erased/shutdown,
 *         if processing ended by an stop signal or if it ran out of events
 */
template<class Scope>
EventSignal EventQueue<Scope>::process_events(Scope& target) {
    release_buffer();
    for(unsigned i = 0; i < m_event_pipeline.size(); i++) {
        SmartEvent<Scope>& event = m_event_pipeline[i];
        EventSignal signal = event->process(target);
        if(signal == EventSignal::stop || signal == EventSignal::erase) {
            return signal;
        }
        else if(signal == EventSignal::end || signal == EventSignal::abort) {
            // delete m_event_pipeline[i];
            m_event_pipeline.erase(m_event_pipeline.begin() + i);
            i--;
        }
    }
    if(is_empty()) {
        return EventSignal::end;
    }
    else {
        return EventSignal::next;
    }
}

/**
 * @brief Adds the event to the targets event queue
 * @param event The event to be added
 * @note The position where the event is added corresponds to its priority value
 * @note We assume that the passed in events are cloned/we manage their memory now!
 */
template<class Scope>
void EventQueue<Scope>::add_event_internal(SmartEvent<Scope> event) {
    if(!is_blocked(event->get_type()) && !is_blocked(event->get_name())
       && !is_blocked(event->get_cause().get_key())
       && !in_cooldown(event->get_type()) && !in_cooldown(event->get_name())) {
        if(!is_empty()) {
            auto it = m_event_pipeline.end();
            do {
                --it;
                if((*it)->get_priority() >= event->get_priority()) {
                    ++it;
                    m_event_pipeline.insert(it, event);
                    return;
                }
            } while(it != m_event_pipeline.begin());
        }
        m_event_pipeline.insert(m_event_pipeline.begin(), event);
        return;
    }
}

/**
 * @brief Deletes all events with given name or type except one
 * @param name The individual name or type of the event
 * @param except The event which shouldn't be deleted
 * @return the count of events which have been deleted
 */
template<class Scope>
unsigned EventQueue<Scope>::scrap_event(std::string name, SmartEvent<Scope>* except) {
    unsigned counter = 0;
    for(unsigned i = 0; i < m_event_pipeline.size(); i++) {
        if(( m_event_pipeline[i]->get_type() == name || m_event_pipeline[i]->get_name() == name) && &m_event_pipeline[i] != except) {
            // delete m_event_pipeline[i];
            m_event_pipeline.erase(m_event_pipeline.begin() + i);
            i--;
            counter++;
        }
    }
    return counter;
}
/**
 * @brief Return if event pipeline is blocked for a specific event
 * @param name Name of the event or event type
 * @return @c bool indicating if event or event type is currently blocked
 */
template<class Scope>
bool EventQueue<Scope>::is_blocked(std::string name) const {
    if(m_block.find(name) == m_block.end()) {
        return false;
    }
    else {
        return m_block.at(name);
    }
}

/**
 * @brief Return if event pipeline is blocked for a specific key
 * @param key The key which gets checked
 * @return @c bool indicating if key is currently blocked
 */
template<class Scope>
bool EventQueue<Scope>::is_blocked(const SDL_Keysym& key) const {
    if(m_block_key.find(key.sym) == m_block_key.end()) {
        return false;
    }
    else {
        return m_block_key.at(key.sym);
    }
}

/**
 * @brief Return if event pipeline is on cooldown for a specific event
 * @param name Name of the event or event type
 * @return @c bool indicating if event or event type is currently on cooldown
 */
template<class Scope>
bool EventQueue<Scope>::in_cooldown(std::string name) const {
    if(m_timestamp.find(name) == m_timestamp.end()) {
        return false;
    }
    if(m_timestamp.at(name) > SDL_GetTicks()) {
        return true;
    }
    else {return false;}
}

/**
 * @brief Adds all buffered events to the pipeline
 */
template<class Scope>
void EventQueue<Scope>::release_buffer() {
    for(SmartEvent<Scope>& event : m_event_buffer) {
        add_event_internal(event);
    }
    m_event_buffer.clear();
}


#endif // EVENT_QUEUE_HPP_INCLUDED
