/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#ifndef ME_TEMPLATE_LAYER_HPP_INCLUDED
#define ME_TEMPLATE_LAYER_HPP_INCLUDED

#include <vector>
#include <string>

#include "event/event.hpp"
#include "event/property_listener.hpp"
#include "util/game_types.hpp"

class MapData;

/**
 * @brief Write something
 */
class MeTemplateLayer : public Event<MapData>{
    public:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source, MapData& base_map) override;
        EventSignal process(MapData& scope) override;

        // Covariant return type!
        MeTemplateLayer* clone() const override {return new MeTemplateLayer(*this);}

        std::string get_type() const override {return m_alias;}

    private:
        static const bool good;
        static const std::string m_alias;
        PropertyListener<MeTemplateLayer> m_property_listener;
        // vv Add members with default values
        bool m_inject = true;
        bool m_purge = false;
        std::string m_destination, m_source;

};

#endif // ME_TEMPLATE_LAYER_HPP_INCLUDED
