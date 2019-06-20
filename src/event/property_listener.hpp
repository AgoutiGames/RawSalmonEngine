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

#include "actor/data_block.hpp"
// #include "actor/actor.hpp"
// #include "map/mapdata.hpp"
// #include "core/gameinfo.hpp"

template<class EventType>
class PropertyListener {
    public:
        void register_variable(std::string variable, int EventType::* member) {m_int_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, float EventType::* member) {m_float_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, bool EventType::* member) {m_bool_link.push_back(std::make_pair(variable, member));}
        void register_variable(std::string variable, std::string EventType::* member) {m_string_link.push_back(std::make_pair(variable, member));}

        void listen(const DataBlock& data, EventType& event) const;

        /*
        void listen(Actor& data, EventType& event) const;
        void listen(MapData& data, EventType& event) const;
        void listen(GameInfo& data, EventType& event) const;
        */
    private:
        std::vector<std::pair<std::string, int EventType::*>> m_int_link;
        std::vector<std::pair<std::string, float EventType::*>> m_float_link;
        std::vector<std::pair<std::string, bool EventType::*>> m_bool_link;
        std::vector<std::pair<std::string, std::string EventType::*>> m_string_link;
};

template<class EventType>
void PropertyListener<EventType>::listen(const DataBlock& data, EventType& event) const {
    for(auto& i : m_int_link) {
        if(data.check_val_int(i.first)) {
            event.*(i.second) = data.get_val_int(i.first);
        }
        else if(data.check_val_float(i.first)) {
            event.*(i.second) = static_cast<int>(data.get_val_float(i.first));
        }
    }

    for(auto& f : m_float_link) {
        if(data.check_val_float(f.first)) {
            event.*(f.second) = data.get_val_float(f.first);
        }
        else if(data.check_val_int(f.first)) {
            event.*(f.second) = static_cast<float>(data.get_val_int(f.first));
        }
    }
    for(auto& b : m_bool_link) {
        if(data.check_val_int(b.first)) {
            event.*(b.second) = data.get_val_bool(b.first);
        }
    }
    for(auto& s : m_string_link) {
        if(data.check_val_int(s.first)) {
            event.*(s.second) = data.get_val_string(s.first);
        }
    }
}

/*
template<class EventType>
void PropertyListener<EventType>::listen(Actor& data, EventType& event) const {
    MapData& map_data = data.get_map();
    GameInfo& game_data = map_data.get_game();
    listen(game_data, event);
    listen(map_data, event);
    listen(data.get_data(), event);
}

template<class EventType>
void PropertyListener<EventType>::listen(MapData& data, EventType& event) const {
    GameInfo& game_data = data.get_game();
    listen(game_data, event);
    listen(data.get_data(), event);
}

template<class EventType>
void PropertyListener<EventType>::listen(GameInfo& data, EventType& event) const {
    listen(data.get_data(), event);
}
*/

#endif // PROPERTY_LISTENER_HPP_INCLUDED
