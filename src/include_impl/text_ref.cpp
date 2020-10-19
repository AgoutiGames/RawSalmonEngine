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

Text::Text(internal::PrimitiveText& impl) : m_impl{&impl} {}
Text::Text(internal::PrimitiveText* impl) : m_impl{impl} {}

bool Text::good() const {return (m_impl == nullptr) ? false : true ;}

std::string Text::get_text() {return m_impl->get_text();}
Text::Attributes Text::get_attributes() {
    internal::PrimitiveText::Attributes a = m_impl->get_attributes();
    return {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
}
int Text::get_w() {return m_impl->get_w();}
int Text::get_h() {return m_impl->get_h();}

void Text::set_text(std::string text) {m_impl->set_text(text);}
void Text::set_attributes(Attributes a) {
    internal::PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
    m_impl->set_attributes(temp);
}

bool Text::apply() {return m_impl->generate_texture();}

bool Text::get_static_mode() const {return m_impl->get_static_mode();}
void Text::set_static_mode(bool mode) {m_impl->set_static_mode(mode);}

bool Text::get_hidden() const {return m_impl->get_hidden();}
void Text::set_hidden(bool mode) {m_impl->set_hidden(mode);}

std::string Text::get_name() {return m_impl->get_name();}

float Text::get_x() {return m_impl->get_x();}
float Text::get_y() {return m_impl->get_y();}

void Text::set_x(float x) {m_impl->set_x(x);}
void Text::set_y(float y) {m_impl->set_y(y);}
