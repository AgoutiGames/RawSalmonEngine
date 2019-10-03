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
#ifndef SMART_HPP_INCLUDED
#define SMART_HPP_INCLUDED

/**
 * @brief This class wraps pointers of "Type" via RAII into an object which behaves more like POD.
 *        Use this class when different derived objects are managed through pointers to base.
 * @note The base class of "Type" MUST supply a clone function which returns a pointer to a hard copy.
 */
template<class Type>
class Smart {
public:
    Smart() : m_internal{nullptr} {}
    /// Constructor taking ownership of object
    Smart(Type* object) : m_internal{object} {}
    ~Smart() {purge();}

    Smart(const Smart& other) : m_internal{other.valid() ? other->clone() : nullptr} {}
    Smart& operator=(const Smart& other);

    Smart(Smart&& other) : m_internal{other.m_internal} {other.m_internal = nullptr;}
    Smart& operator=(Smart&& other);

    Type* operator->() const {return m_internal;}
    Type& operator*() const {return *m_internal;}

    bool valid() const {return (m_internal == nullptr) ? false : true;}
    explicit operator bool() const {return valid();}

    Type* get() const {return m_internal;}

private:
    void purge() {if(valid()) {delete m_internal;}}

    Type* m_internal;
};

template<class Scope>
Smart<Scope>& Smart<Scope>::operator=(const Smart<Scope>& other) {
    if(this != &other) {
        purge();
        m_internal = other.valid() ? other->clone() : nullptr;
    }
    return *this;
}

template<class Scope>
Smart<Scope>& Smart<Scope>::operator=(Smart<Scope>&& other) {
    if(this != &other) {
        purge();
        m_internal = other.m_internal;
        other.m_internal = nullptr;
    }
    return *this;
}


#endif // SMART_HPP_INCLUDED
