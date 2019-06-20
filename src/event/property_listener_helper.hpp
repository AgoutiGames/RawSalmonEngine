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
#ifndef PROPERTY_LISTENER_HELPER_HPP_INCLUDED
#define PROPERTY_LISTENER_HELPER_HPP_INCLUDED

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "core/gameinfo.hpp"
#include "event/property_listener.hpp"

template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, Actor& actor);

template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, MapData& mapdata);

template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, GameInfo& game);


template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, Actor& actor) {
    MapData& map_data = actor.get_map();
    listen(listener, event, map_data);
    listener.listen(actor.get_data(), event);
}

template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, MapData& mapdata) {
    GameInfo& game_data = mapdata.get_game();
    listen(listener, event, game_data);
    listener.listen(mapdata.get_data(), event);
}

template <class EventType>
void listen(PropertyListener<EventType>& listener, EventType& event, GameInfo& game) {
    listener.listen(game.get_data(), event);
}

#endif // PROPERTY_LISTENER_HELPER_HPP_INCLUDED
