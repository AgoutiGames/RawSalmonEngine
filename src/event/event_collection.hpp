#ifndef EVENT_COLLECTION_HPP_INCLUDED
#define EVENT_COLLECTION_HPP_INCLUDED

#include <map>
#include <string>

template <class Scope>
class SmartEvent;

template<class Scope, class Key=std::string>
class EventCollection {
    public:
        void register_event(std::pair<Key, SmartEvent<Scope>> key_event_pair) {m_events.insert(key_event_pair);}
        void register_event(Key name, SmartEvent<Scope> event) {register_event(std::make_pair(name, event));}
        SmartEvent<Scope> get_event(Key name) const {return m_events.at(name);}
        bool check_event(Key name) const { return m_events.find(name) != m_events.end(); }

        std::map<Key, SmartEvent<Scope>>& get_container() {return m_events;}
    private:
        std::map<Key, SmartEvent<Scope>> m_events;
};

#endif // EVENT_COLLECTION_HPP_INCLUDED
