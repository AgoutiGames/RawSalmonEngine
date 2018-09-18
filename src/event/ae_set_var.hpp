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
#ifndef AE_SET_VAR_HPP_INCLUDED //< Change this
#define AE_SET_VAR_HPP_INCLUDED //< Change this

#include <vector>
#include <string>

#include "event/actor_event.hpp"
#include "event/event_container.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Sets custom vars of actor or map
 */
class AeSetVar : public EventContainer<ActorEvent, AeSetVar>{
    // The default interface block (copy this!)
    public:
        AeSetVar() {}
        static AeSetVar* create() {return duplicate(AeSetVar());}
        virtual tinyxml2::XMLError parse(tinyxml2::XMLElement* source, MapData& map, std::pair<std::string, ActorEvent*>& entry) const override; //<Define this!
        virtual EventSignal process(Actor& actor) override;     //< Define this!
        virtual ~AeSetVar() override {}
        virtual std::string get_type() const override {return m_alias;}
        static std::string get_type_static() {return m_alias;}
        using EventContainer::kill;
        virtual void kill() override {kill(this);}
        virtual ActorEvent* copy() const override {return duplicate(this);}
    private:
        static std::string m_alias; //< Define this!
    // The specialized block
    public:
        AeSetVar(std::string name, bool data, bool global = false);
        AeSetVar(std::string name, int data, bool global = false, bool add = false, bool mult = false);
        AeSetVar(std::string name, float data, bool global = false, bool add = false, bool mult = false);
        AeSetVar(std::string name, std::string data, bool global = false, bool add = false);
        static AeSetVar* create(std::string name, bool data, bool global = false);
        static AeSetVar* create(std::string name, int data, bool global = false, bool add = false, bool mult = false);
        static AeSetVar* create(std::string name, float data, bool global = false, bool add = false, bool mult = false);
        static AeSetVar* create(std::string name, std::string data, bool global = false, bool add = false);
    private:
        // Members
        std::string m_name;
        bool m_global;
        bool m_add;
        bool m_mult;
        struct Value {
            enum Type{
                Bool,
                Int,
                Float,
                String,
                undefined
            } type;

            union{
                bool b;
                int i;
                float f;
            };
            std::string s;
        } m_value;
};

#endif // AE_SET_VAR_HPP_INCLUDED
