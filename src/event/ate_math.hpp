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
#ifndef ATE_MATH_HPP_INCLUDED
#define ATE_MATH_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Write something
 */
class AteMath : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& scope) override;

        // Covariant return type!
        AteMath* create() const override {return new AteMath();}
        AteMath* clone() const override {return new AteMath(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AteMath> m_property_listener;
        // vv Add members with default values
        bool m_equals = true;
        bool m_bigger = false;
        bool m_smaller = false;
        bool m_not_equal = false;

        PropertyType m_type1 = PropertyType::Float;
        bool m_bool1 = false;
        int m_int1 = 0;
        float m_float1 = 0.0f;
        std::string m_string1 = "";

        PropertyType m_type2 = PropertyType::Float;
        bool m_bool2 = false;
        int m_int2 = 0;
        float m_float2 = 0.0f;
        std::string m_string2 = "";

};

#endif // ATE_MATH_HPP_INCLUDED
