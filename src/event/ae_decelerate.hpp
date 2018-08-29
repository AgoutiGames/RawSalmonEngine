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
#ifndef AE_DECELERATE_HPP_INCLUDED //< Change this
#define AE_DECELERATE_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Lower speed var of actor until 0
 */
class AeDecelerate : public EventContainer<ActorEvent, AeDecelerate>{
    // The default interface block (copy this!)
    public:
        AeDecelerate() {}
        static AeDecelerate* create() {return duplicate(AeDecelerate());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeDecelerate() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() override {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeDecelerate(float x_factor, float y_factor, std::string x_name = "__XSPEED", std::string y_name = "__YSPEED");
        static AeDecelerate* create(float x_factor, float y_factor, std::string x_name = "__XSPEED", std::string y_name = "__YSPEED");
    private:
        // Members
        float m_x_factor;
        float m_y_factor;
        std::string m_x_speed_name;
        std::string m_y_speed_name;
};

#endif // AE_DECELERATE_HPP_INCLUDED