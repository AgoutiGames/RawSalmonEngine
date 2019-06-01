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
#include <memory>

#include "event/smart_event.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Trigger event if actor is or isn't on ground
 */
class AteOnGround : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AteOnGround* create() const override {return new AteOnGround();}
        AteOnGround* clone() const override {return new AteOnGround(*this);}

        std::string get_type() const override {return m_alias;}

        // reimplement/hide inherited function
        void set_cause(Cause x);
        /*
        AteOnGround() = default;
        AteOnGround(const AteOnGround& other) : Event<Actor>(){
            *this = other;
        }
        AteOnGround& operator=(const AteOnGround& other) {
            Event<Actor>::operator=(other);
            m_decision = other.m_decision;
            m_tolerance = other.m_tolerance;
            m_start = other.m_start;
            m_continuous = other.m_continuous;
            m_direction = other.m_direction;
            if(other.m_success != nullptr) {
                m_success = std::unique_ptr<Event<Actor>>(other.m_success->clone());
            }
            if(other.m_failure != nullptr) {
                m_failure = std::unique_ptr<Event<Actor>>(other.m_failure->clone());
            }
            return *this;
        }
        */

    private:
        static const bool good;
        static const std::string m_alias; //< Define this!
        // vv Add members with default values
        //std::unique_ptr<Event<Actor>> m_success = nullptr;
        //std::unique_ptr<Event<Actor>> m_failure = nullptr;
        SmartEvent<Actor> m_success;
        SmartEvent<Actor> m_failure;
        Direction m_direction = Direction::down;
        bool m_continuous = false;
        bool m_start = true;
        int m_tolerance = 0;
        bool m_decision;
};

#endif // ATE_ON_GROUND_HPP_INCLUDED
