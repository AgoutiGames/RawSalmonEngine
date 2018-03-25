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
#ifndef AE_MOVE_DIRECTION_HPP_INCLUDED
#define AE_MOVE_DIRECTION_HPP_INCLUDED

#include <map>
#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

class Actor;

/**
 * @brief Move the actor to the supplied direction for x frames
 */
class AeMoveDirection : public EventContainer<ActorEvent, AeMoveDirection>{
    // The default interface block (copy this!)
    public:
        AeMoveDirection() {}
        static AeMoveDirection* create() {return duplicate(AeMoveDirection());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeMoveDirection() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeMoveDirection(Direction dir, unsigned duration = 1);
        static AeMoveDirection* create(Direction dir, unsigned duration = 1);
    private:
        Direction m_direction;
        unsigned m_duration; ///< How many frames the movement persists
};

#endif // AE_MOVE_DIRECTION_HPP_INCLUDED
