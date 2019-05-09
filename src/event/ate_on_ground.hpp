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
#ifndef ATE_ON_GROUND_HPP_INCLUDED //< Change this
#define ATE_ON_GROUND_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Trigger event if actor is or isn't on ground
 */
class AteOnGround : public EventContainer<ActorEvent, AteOnGround>{
    // The default interface block (copy this!)
    public:
        AteOnGround() {}
        static AteOnGround* create() {return duplicate(AteOnGround());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AteOnGround() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() override;
        virtual ActorEvent* copy() const override;
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AteOnGround(ActorEvent* success, ActorEvent* failure, Direction direction = Direction::down, bool continuous = false, int tolerance = 0);
        static AteOnGround* create(ActorEvent* success, ActorEvent* failure, Direction direction = Direction::down, bool continuous = false, int tolerance = 0);
        virtual void set_cause(Cause x) override;
        virtual void set_key(SDL_Keysym x) override;
    private:
        ActorEvent* m_success;
        ActorEvent* m_failure;
        Direction m_direction;
        bool m_continuous;
        bool m_decision;
        bool m_start = true;
        int m_tolerance;
};

#endif // ATE_ON_GROUND_HPP_INCLUDED
