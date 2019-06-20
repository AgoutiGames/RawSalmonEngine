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
#ifndef ME_ERASE_ACTOR_HPP_INCLUDED
#define ME_ERASE_ACTOR_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class MapData;

/**
 * @brief Write something
 */
class MeEraseActor : public Event<MapData>{
    public:

        MeEraseActor() = default;
        MeEraseActor(Actor* pointer, std::string event_name) : m_actor_pointer{pointer} {m_name = event_name;}

        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(MapData& scope) override;

        // Covariant return type!
        MeEraseActor* create() const override {return new MeEraseActor();}
        MeEraseActor* clone() const override {return new MeEraseActor(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<MeEraseActor> m_property_listener;
        // vv Add members with default values
        std::string m_actor_name = "";
        Actor* m_actor_pointer = nullptr;

};

#endif // ME_ERASE_ACTOR_HPP_INCLUDED
