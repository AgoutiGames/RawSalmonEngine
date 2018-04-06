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
#ifndef AE_ANIMATE_HPP_INCLUDED //< Change this
#define AE_ANIMATE_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Animate the actor
 */
class AeAnimate : public EventContainer<ActorEvent, AeAnimate>{
    // The default interface block (copy this!)
    public:
        AeAnimate() {}
        static AeAnimate* create() {return duplicate(AeAnimate());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeAnimate() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeAnimate(AnimationType anim, Direction dir, unsigned cyc, unsigned a_f, unsigned g_f);
        static AeAnimate* create(AnimationType anim, Direction dir, unsigned cyc, unsigned a_f, unsigned g_f);
    private:
        AnimationType m_animation;
        Direction m_direction;
        unsigned m_cycles;
        unsigned m_anim_frames;
        unsigned m_game_frames;
        // Members
};

#endif // AE_ANIMATE_HPP_INCLUDED
