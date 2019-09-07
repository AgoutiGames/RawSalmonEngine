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
#ifndef ATTRIBUTE_PARSER_HPP_INCLUDED
#define ATTRIBUTE_PARSER_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <tinyxml2.h>

#include "util/game_types.hpp"

/**
 * @brief A helper class which parses XML_Element Attributes
 */
class AttributeParser{
    public:
        void add(bool& value, std::string name) {m_bool[name] = &value;}
        void add(unsigned& value, std::string name) {m_unsigned[name] = &value;}
        void add(int& value, std::string name) {m_int[name] = &value;}
        void add(float& value, std::string name) {m_float[name] = &value;}
        void add(std::string& value, std::string name) {m_string[name] = &value;}
        tinyxml2::XMLError parse(tinyxml2::XMLElement* source, bool ignore_missing = false);

    private:
        std::map<std::string, bool*> m_bool;
        std::map<std::string, unsigned*> m_unsigned;
        std::map<std::string, int*> m_int;
        std::map<std::string, float*> m_float;
        std::map<std::string, std::string*> m_string;
};

#endif // ATTRIBUTE_PARSER_HPP_INCLUDED
