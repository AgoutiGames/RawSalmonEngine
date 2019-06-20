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

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class Actor;

/**
 * @brief Sets custom vars of actor or map
 */
class AeSetVar : public Event<Actor>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(Actor& actor) override;

        // Covariant return type!
        AeSetVar* create() const override {return new AeSetVar();}
        AeSetVar* clone() const override {return new AeSetVar(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<AeSetVar> m_property_listener;
        // vv Add members with default values
        std::string m_val_name = "";
        bool m_add = false;
        bool m_mult = false;
        struct Value {
            enum Type{
                Bool,
                Int,
                Float,
                String
            };
            Type type = Float;

            union{
                bool b;
                int i;
                float f;
            };
            std::string s;
        } m_value;
};

#endif // AE_SET_VAR_HPP_INCLUDED
