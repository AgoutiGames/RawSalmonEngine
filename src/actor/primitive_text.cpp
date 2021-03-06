/*
 * Copyright 2017-2020 Agouti Games Team (see the AUTHORS file)
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
#include "actor/primitive_text.hpp"

#include <algorithm>
#include <iostream>
#include <tinyxml2.h>

#include "map/mapdata.hpp"
#include "core/gameinfo.hpp"
#include "util/game_types.hpp"
#include "util/attribute_parser.hpp"
#include "util/logger.hpp"

namespace salmon { namespace internal {

PrimitiveText::PrimitiveText(Rect r, std::string text, Attributes atr, MapData& mapdata, std::string name)
: Primitive(mapdata, name), m_mapdata{&mapdata}, m_text{text}, m_attributes{atr} {
    m_transform.set_origin(0,0);
    m_transform.set_pos(r.x,r.y);
    m_transform.set_dimensions(r.w,r.h);
    generate_texture();
}

bool PrimitiveText::render(float x_cam, float y_cam) const {
    if(m_hidden) {return true;}
    PixelDimensions dim = m_transform.get_base_dimensions();
    SDL_Rect rect{0,0,dim.w,dim.h};
    Rect r = m_transform.to_rect();
    r.x -= x_cam;
    r.y -= y_cam;
    SDL_Rect dest = make_rect(r);
    m_texture.render_resize(&rect, &dest);
    return true;
}

PrimitiveText* PrimitiveText::parse(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;
    XMLError eResult;
    AttributeParser parser;
    float x, y, w, h;
    std::string name;
    parser.add(x, "x");
    parser.add(y, "y");
    parser.add(w, "width");
    parser.add(h, "height");
    parser.add(name, "name");
    eResult = parser.parse(source);
    if(eResult != XML_SUCCESS) {return nullptr;}

    source = source->FirstChildElement("text");
    // Reset parser
    parser = AttributeParser();

    std::string color = "#000000";

    PrimitiveText::Attributes atr;

    parser.add(atr.font_family, "fontfamily");
    parser.add(color, "color");
    parser.add(atr.pixel_size, "pixelsize");
    parser.add(atr.wrap, "wrap");
    parser.add(atr.bold, "bold");
    parser.add(atr.italic, "italic");
    parser.add(atr.underline, "underline");
    parser.add(atr.strikeout, "strikeout");
    parser.add(atr.kerning, "kerning");
    eResult = parser.parse(source, true);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing text attributes";
        return nullptr;
    }
    atr.color = str_to_color(color);
    const char* text_p = source->GetText();
    if(text_p == nullptr) {
        Logger(Logger::error) << "Text of text primitive is missing!";
        return nullptr;
    }

    return new PrimitiveText({x,y,w,h},std::string(text_p), atr, base_map, name);
}

bool PrimitiveText::generate_texture() {
    TTF_Font* font = m_mapdata->get_game().get_font_manager().get_font(m_attributes.font_family,m_attributes.pixel_size);
    if(font == nullptr) {return false;}

    TTF_SetFontKerning(font, m_attributes.kerning);
    int style = TTF_STYLE_NORMAL;
    if(m_attributes.bold) {style += TTF_STYLE_BOLD;}
    if(m_attributes.italic) {style += TTF_STYLE_ITALIC;}
    if(m_attributes.underline) {style += TTF_STYLE_UNDERLINE;}
    if(m_attributes.strikeout) {style += TTF_STYLE_STRIKETHROUGH;}
    TTF_SetFontStyle(font, style);

    if(m_attributes.wrap) {
        m_texture.loadFromRenderedText(m_renderer, m_text, m_attributes.color, font, m_transform.get_base_dimensions().w);
    }
    else {
        m_texture.loadFromRenderedText(m_renderer, m_text, m_attributes.color, font);
    }
    m_transform.set_dimensions(m_texture.getWidth(),m_texture.getHeight());
    return true;
}

}} // namespace salmon::internal
