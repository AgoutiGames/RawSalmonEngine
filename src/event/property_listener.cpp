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
 #include "event/property_listener.hpp"

 #include "actor/data_block.hpp"
 #include "actor/actor.hpp"
 #include "map/mapdata.hpp"
 #include "core/gameinfo.hpp"

void PropertyListener::listen(const DataBlock& data) const {
    for(auto& i : m_int_link) {
        if(data.check_val_int(i.first)) {
            *i.second = data.get_val_int(i.first);
        }
        else if(data.check_val_float(i.first)) {
            *i.second = static_cast<int>(data.get_val_float(i.first));
        }
    }

    for(auto& f : m_float_link) {
        if(data.check_val_float(f.first)) {
            *f.second = data.get_val_float(f.first);
        }
        else if(data.check_val_int(f.first)) {
            *f.second = static_cast<float>(data.get_val_int(f.first));
        }
    }
    for(auto& b : m_bool_link) {
        if(data.check_val_int(b.first)) {
            *b.second = data.get_val_bool(b.first);
        }
    }
    for(auto& s : m_string_link) {
        if(data.check_val_int(s.first)) {
            *s.second = data.get_val_string(s.first);
        }
    }
}

void PropertyListener::listen(Actor& data) const {
    MapData& map_data = data.get_map();
    GameInfo& game_data = map_data.get_game();
    listen(game_data);
    listen(map_data);
    listen(data.get_data());
}

void PropertyListener::listen(MapData& data) const {
    GameInfo& game_data = data.get_game();
    listen(game_data);
    listen(data.get_data());
}

void PropertyListener::listen(GameInfo& data) const {
    listen(data.get_data());
}
