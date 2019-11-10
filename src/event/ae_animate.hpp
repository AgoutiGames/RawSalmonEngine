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
#include "event/property_listener.hpp"
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
        AeAnimate* clone() const override {return new AeAnimate(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AeAnimate> m_property_listener;
        // vv Add members with default values
        bool m_first = true;
        salmon::AnimationType m_animation = salmon::AnimationType::idle;
        salmon::Direction m_direction = salmon::Direction::current;
        int m_cycles = 0;
        int m_anim_frames = 0;
        int m_game_frames = 0;
        float m_speed = 1.0f;
        bool m_stop_on_last_frame = false;
        // Members
};

#endif // AE_ANIMATE_HPP_INCLUDED
