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
#include "event/ae_spawn_text.hpp"

#include <string>
#include <map>
#include <iostream>

#include "actor/actor.hpp"
#include "actor/primitive_text.hpp"
#include "event/property_parser.hpp"
#include "map/mapdata.hpp"
#include "map/object_layer.hpp"
#include "event/property_listener_helper.hpp"
#include "util/game_types.hpp"

const std::string AeSpawnText::m_alias = "AeSpawnText";

const bool AeSpawnText::good = Event<Actor>::register_class<AeSpawnText>();

/**
 * @brief Spawn text with the supplied attributes
 * @param Actor The Actor to which the text is relative positioned
 * @return @c EventSignal which can halt event processing, delete this event, etc.
 */
EventSignal AeSpawnText::process(Actor& scope) {
    // Syncs members with possibly linked DataBlock variables
    listen(m_property_listener, *this, scope);

    // Check if target layer exists
    auto obj_layers = scope.get_map().get_layer_collection().get_object_layers();
    ObjectLayer* target_layer = nullptr;
    for(auto layer : obj_layers) {
        if(layer->get_name() == m_layer_name) {
            target_layer = layer;
            break;
        }
    }
    if(target_layer == nullptr) {
        Logger(Logger::error) << info() << " failed to find layer: " << m_layer_name;
        return EventSignal::abort;
    }
    PrimitiveText::Attributes atr;
    atr.bold = m_bold;
    atr.color = str_to_color(m_color);
    atr.font_family = m_font;
    atr.italic = m_italic;
    atr.kerning = m_kerning;
    atr.pixel_size = static_cast<unsigned>(m_font_size);
    atr.strikeout = m_strikeout;
    atr.underline = m_underline;
    atr.wrap = m_wrap;

    int x_pos, y_pos;
    if(m_center) {
        x_pos = static_cast<int>(scope.get_x_center() + m_x_pos);
        y_pos = static_cast<int>(scope.get_y_center() + m_y_pos);
    }
    else {
        x_pos = static_cast<int>(scope.get_x() + m_x_pos);
        y_pos = static_cast<int>(scope.get_y() + m_y_pos);
    }

    Primitive* text_primitive = new PrimitiveText(x_pos, y_pos,
                                                  m_width, m_height, m_text, atr, scope.get_map(), m_primitive_name);

    target_layer->add_primitive(text_primitive);
    return EventSignal::end;
}

/**
 * @brief Parse event from symbolic tile
 * @param source The symbolic tile XMLElement
 * @param base_map Seldomly used in parser to fetch Actors or other events
 * @return @c XMLError indication sucess or failure of parsing
 */
tinyxml2::XMLError AeSpawnText::init(tinyxml2::XMLElement* source, MapData& base_map) {
    // Mute unused parameter warning
    (void) base_map;
    using namespace tinyxml2;

    PropertyParser<AeSpawnText> parser(m_property_listener, *this);

    parser.add(m_name, "NAME");
    parser.add(m_priority, "PRIORITY");
    parser.add(&AeSpawnText::m_font, "FONT");
    parser.add(&AeSpawnText::m_font_size, "FONT_SIZE");
    parser.add(&AeSpawnText::m_x_pos, "X_POS");
    parser.add(&AeSpawnText::m_y_pos, "Y_POS");
    parser.add(&AeSpawnText::m_width, "WIDTH");
    parser.add(&AeSpawnText::m_height, "HEIGHT");
    parser.add(&AeSpawnText::m_text, "TEXT");
    parser.add(&AeSpawnText::m_bold, "BOLD");
    parser.add(&AeSpawnText::m_italic, "ITALIC");
    parser.add(&AeSpawnText::m_underline, "UNDERLINE");
    parser.add(&AeSpawnText::m_strikeout, "STRIKEOUT");
    parser.add(&AeSpawnText::m_kerning, "KERNING");
    parser.add(&AeSpawnText::m_wrap, "WRAP");
    parser.add(&AeSpawnText::m_primitive_name, "PRIMITIVE_NAME");
    parser.add(&AeSpawnText::m_layer_name, "LAYER_NAME");
    parser.add(&AeSpawnText::m_color, "COLOR");
    parser.add(&AeSpawnText::m_center, "CENTER_ORIGIN");

    XMLError eResult = parser.parse(source);

    if(m_text == "") {
        Logger(Logger::error) << "Missing text property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(m_name == "") {
        Logger(Logger::error) << "Missing name property!";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing event: \"" << m_name << "\"";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }

    return XML_SUCCESS;
}
