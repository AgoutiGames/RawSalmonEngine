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
#ifndef AE_JUMP_HPP_INCLUDED //< Change this
#define AE_JUMP_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Perform a jump
 */
class AeJump : public EventContainer<ActorEvent, AeJump>{
    // The default interface block (copy this!)
    public:
        AeJump() {}
        static AeJump* create() {return duplicate(AeJump());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeJump() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!

    // The specialized block
    public:
        AeJump(float dur, float j_h, bool slow_r, float slow_f, AnimationType anim, Direction anim_dir);
        static AeJump* create(float dur, float j_h, bool slow_r, float slow_f, AnimationType anim, Direction anim_dir);
    private:
        float m_duration;
        float m_jump_height;

        bool m_slow_on_release;
        float m_slow_factor;

        AnimationType m_animation;
        Direction m_anim_dir;

        float m_speed = 0;
        float m_deceleration = 0;
        bool m_started = false;
        // Members
};

#endif // AE_JUMP_HPP_INCLUDED
