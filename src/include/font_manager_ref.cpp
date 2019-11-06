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
#include "include/font_manager_ref.hpp"

#include "core/font_manager.hpp"

using salmon::FontManagerRef;

FontManagerRef::FontManagerRef(FontManager& impl) : m_impl{&impl} {}

TTF_Font* FontManagerRef::get_font(std::string name, int pt_size) {return m_impl->get_font(name, pt_size);}
void FontManagerRef::clear() {m_impl->clear();}
