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
#ifndef TEXT_REF_HPP_INCLUDED
#define TEXT_REF_HPP_INCLUDED

#include <string>

#include "./transform.hpp"

namespace salmon {

namespace internal{class PrimitiveText;}

class Text {
    friend class MapData;
    public:
        Text(internal::PrimitiveText& impl);
        Text(internal::PrimitiveText* impl);

        /// Returns true if TextRef exists and is valid
        bool good() const;

        /// Describes visual features of text
        struct Attributes {
            std::string font_family = "sans-serif";
            struct {
                // RGB values range from 0 to 255
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;
                unsigned char a = 255;
            } color;
            unsigned pixel_size = 12;
            bool wrap = false, bold = false, italic = false, underline = false, strikeout = false, kerning = true;
        };

        /// Returns the name which identifies this text object
        std::string get_name();

        /// Returns reference to current transform
        Transform& get_transform();

        /// Returns true if text is currently hidden
        bool get_hidden() const;
        /// If mode is true the text won't be rendered, when false is shows up again
        void set_hidden(bool mode);

        /// Returns the text which gets displayed as a string
        std::string get_text();
        /// Returns the attribute object describing the visual appearance of the text
        Attributes get_attributes();

        /// Sets the displayed text to supplied string
        void set_text(std::string text);
        /// Sets the visual appearance according to supplied attribute object
        void set_attributes(Attributes a);

        /// Applies the changes to the TextRef object after changing text or attributes
        bool apply();

    private:
        internal::PrimitiveText* m_impl;
};
}


#endif // TEXT_REF_HPP_INCLUDED
