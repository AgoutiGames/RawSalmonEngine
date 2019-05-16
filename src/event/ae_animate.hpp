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

#include "event/event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Animate the actor
 * @note Due to late checking this event stays one extra game frame in the event pipeline
 *       This means that an event with frames = 1 animates in the first and deletes in the second frame
 *       If placed in "OnIdle" it will effectively only animate each second frame
 */
class AeAnimate : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeAnimate* create() const override {return new AeAnimate();}
        AeAnimate* clone() const override {return new AeAnimate(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias; //< Define this!
        // vv Add members with default values
        AnimationType m_animation = AnimationType::idle;
        Direction m_direction = Direction::current;
        unsigned m_cycles = 0;
        unsigned m_anim_frames = 0;
        unsigned m_game_frames = 0;
        // Members
};

#endif // AE_ANIMATE_HPP_INCLUDED
