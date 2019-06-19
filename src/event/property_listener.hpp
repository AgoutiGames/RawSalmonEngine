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
#ifndef PROPERTY_LISTENER_HPP_INCLUDED
#define PROPERTY_LISTENER_HPP_INCLUDED

#include <map>
#include <vector>
#include <string>

class DataBlock;
class Actor;
class MapData;
class GameInfo;

class PropertyListener {
    public:
        void register_variable(std::string variable, int* member) {m_int_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, float* member) {m_float_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, bool* member) {m_bool_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, std::string* member) {m_string_link.push_back(std::make_pair(variable, member));}

        void listen(const DataBlock& data) const;

        void listen(Actor& data) const;
        void listen(MapData& data) const;
        void listen(GameInfo& data) const;
    private:
        std::vector<std::pair<std::string, int*>> m_int_link;
        std::vector<std::pair<std::string, float*>> m_float_link;
        std::vector<std::pair<std::string, bool*>> m_bool_link;
        std::vector<std::pair<std::string, std::string*>> m_string_link;
};


#endif // PROPERTY_LISTENER_HPP_INCLUDED
