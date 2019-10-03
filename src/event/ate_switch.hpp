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
#ifndef ATE_SWITCH_HPP_INCLUDED
#define ATE_SWITCH_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "event/smart_event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Write something
 */
class AteSwitch : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& scope) override;

        // Covariant return type!
        AteSwitch* clone() const override {return new AteSwitch(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AteSwitch> m_property_listener;
        // vv Add members with default values
        std::string m_success_name;
        std::string m_failure_name;
        std::vector<std::string> m_trigger_events;
        bool m_and = true;
        bool m_or = false;
        bool m_start = true;
        bool m_decision;
        SmartEvent<Actor> m_success;
        SmartEvent<Actor> m_failure;
};

#endif // ATE_SWITCH_HPP_INCLUDED
