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
#ifndef PRIMITIVE_TEXT_HPP_INCLUDED
#define PRIMITIVE_TEXT_HPP_INCLUDED

#include "./types.hpp"
#include "actor/primitive.hpp"
#include "graphics/texture.hpp"

namespace salmon { namespace internal {

class MapData;

class PrimitiveText : public Primitive {
    public:
        struct Attributes {
            std::string font_family = "sans-serif";
            SDL_Color color = {0,0,0,255};
            unsigned pixel_size = 12;
            bool wrap = false, bold = false, italic = false, underline = false, strikeout = false, kerning = true;
        };

        PrimitiveText(Rect r, std::string text, Attributes atr, MapData& mapdata, std::string name = "");

        bool render(float x_cam, float y_cam) const override;
        PrimitiveType get_type() const override {return PrimitiveType::text;}

        // Covariant return type!
        PrimitiveText* clone() const override {return new PrimitiveText(*this);}

        std::string get_text() {return m_text;}
        Attributes get_attributes() {return m_attributes;}

        void set_text(std::string text) {m_text = text;}
        void set_attributes(Attributes attributes) {m_attributes = attributes;}

        bool generate_texture();

        static PrimitiveText* parse(tinyxml2::XMLElement* source, MapData& base_map);
    private:
        MapData* m_mapdata;
        Texture m_texture;

        std::string m_text;
        Attributes m_attributes;
};
}} // namespace salmon::internal

#endif // PRIMITIVE_TEXT_HPP_INCLUDED
