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
#include "text_ref.hpp"

#include "actor/primitive_text.hpp"

using namespace salmon;

TextRef::TextRef(internal::PrimitiveText& impl) : m_impl{&impl} {}
TextRef::TextRef(internal::PrimitiveText* impl) : m_impl{impl} {}

bool TextRef::good() const {return (m_impl == nullptr) ? false : true ;}

std::string TextRef::get_text() {return m_impl->get_text();}
TextRef::Attributes TextRef::get_attributes() {
    internal::PrimitiveText::Attributes a = m_impl->get_attributes();
    return {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
}
int TextRef::get_w() {return m_impl->get_w();}
int TextRef::get_h() {return m_impl->get_h();}

void TextRef::set_text(std::string text) {m_impl->set_text(text);}
void TextRef::set_attributes(Attributes a) {
    internal::PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
    m_impl->set_attributes(temp);
}

bool TextRef::apply() {return m_impl->generate_texture();}

bool TextRef::get_static_mode() const {return m_impl->get_static_mode();}
void TextRef::set_static_mode(bool mode) {m_impl->set_static_mode(mode);}

bool TextRef::get_hidden() const {return m_impl->get_hidden();}
void TextRef::set_hidden(bool mode) {m_impl->set_hidden(mode);}

std::string TextRef::get_name() {return m_impl->get_name();}

float TextRef::get_x() {return m_impl->get_x();}
float TextRef::get_y() {return m_impl->get_y();}

void TextRef::set_x(float x) {m_impl->set_x(x);}
void TextRef::set_y(float y) {m_impl->set_y(y);}
