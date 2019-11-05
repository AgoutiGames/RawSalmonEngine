/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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
#ifndef AE_MULTI_HPP_INCLUDED //< Change this
#define AE_MULTI_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "event/event_queue.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief An event holding a list of other events
 */
class AeMulti : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeMulti* clone() const override {return new AeMulti(*this);}

        std::string get_type() const override {return m_alias;}

        // reimplement/hide inherited function
        void set_collision(Collision x) override;

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AeMulti> m_property_listener;
        // vv Add members with default values
        EventQueue<Actor> m_events;
};

#endif // AE_MULTI_HPP_INCLUDED
