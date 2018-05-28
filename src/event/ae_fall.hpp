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
#ifndef AE_FALL_HPP_INCLUDED //< Change this
#define AE_FALL_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Let the actor fall
 */
class AeFall : public EventContainer<ActorEvent, AeFall>{
    // The default interface block (copy this!)
    public:
        AeFall() {}
        static AeFall* create() {return duplicate(AeFall());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeFall() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() override {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeFall(float acc, float max, unsigned death, Direction fall, AnimationType anim, Direction a_dir);
        static AeFall* create(float acc, float max, unsigned death, Direction fall, AnimationType anim, Direction a_dir);
    private:
        float m_acceleration;
        float m_max_velocity;
        unsigned m_death_height;
        Direction m_fall_dir;

        AnimationType m_animation;
        Direction m_anim_dir;

        float m_speed = 0;
        float m_height = 0;
        // Members
};

#endif // AE_FALL_HPP_INCLUDED
