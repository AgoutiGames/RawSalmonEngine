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
#ifndef ATE_SINGLE_HPP_INCLUDED
#define ATE_SINGLE_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/smart_event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Write something
 */
class AteSingle : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& scope) override;

        // Covariant return type!
        AteSingle* clone() const override {return new AteSingle(*this);}

        std::string get_type() const override {return m_alias;}

        // reimplement/hide inherited function
        void set_cause(Cause x) override;

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AteSingle> m_property_listener;
        // vv Add members with default values
        SmartEvent<Actor> m_event;
        bool m_first_execution = true;

};

#endif // ATE_SINGLE_HPP_INCLUDED
