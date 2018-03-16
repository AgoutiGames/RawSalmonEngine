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
#ifndef AE_MOVE_SUSTAINED_HPP_INCLUDED
#define AE_MOVE_SUSTAINED_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Move the actor to the supplied direction for x frames
 */
class AeMoveSustained : public EventContainer<ActorEvent, AeMoveSustained>{

    public:
        AeMoveSustained(Direction dir = Direction::up, SDL_Keysym key = SDL_Keysym(), Priority prio = Priority::medium);

        virtual EventSignal process(Actor& actor) override;
        virtual ~AeMoveSustained() override;
        virtual std::string get_type() override{return m_alias;}

        using EventContainer::kill;
        virtual void kill() {kill(this);}

        static std::string get_type_static() {return m_alias;}

        static AeMoveSustained* create(Direction dir = Direction::up, SDL_Keysym key = SDL_Keysym(), Priority prio = Priority::medium);

    private:
        static std::string m_alias;

        Direction m_direction = Direction::up;
        SDL_Keysym m_key;
};

#endif // AE_MOVE_SUSTAINED_HPP_INCLUDED
