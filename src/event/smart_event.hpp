#ifndef SMART_EVENT_HPP_INCLUDED
#define SMART_EVENT_HPP_INCLUDED

#include "event/event.hpp"

class MapData;

template<class Scope>
class SmartEvent {
public:
    SmartEvent() : m_internal{nullptr} {}
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
