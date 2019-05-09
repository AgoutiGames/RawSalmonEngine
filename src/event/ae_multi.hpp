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

#include "actor/cause.hpp"
#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief An event holding a list of other events
 */
class AeMulti : public EventContainer<ActorEvent, AeMulti>{
    // The default interface block (copy this!)
    public:
        AeMulti() {}
        static AeMulti* create() {return duplicate(AeMulti());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeMulti() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() override;
        virtual ActorEvent* copy() const override;
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeMulti(std::vector<ActorEvent*> event_list);
        static AeMulti* create(std::vector<ActorEvent*> event_list);
        virtual void set_cause(Cause x) override;
        virtual void set_key(SDL_Keysym x) override;
    private:
        std::vector<ActorEvent*> m_events;
        // Members
};

#endif // AE_MULTI_HPP_INCLUDED
