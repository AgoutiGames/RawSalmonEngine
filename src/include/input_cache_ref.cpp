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
#include "include/input_cache_ref.hpp"

#include "core/input_cache.hpp"

using salmon::InputCacheRef;

InputCacheRef::InputCacheRef(InputCache& impl) : m_impl{&impl} {}

bool InputCacheRef::is_down(std::string key) const {return m_impl->is_down(key);}
bool InputCacheRef::just_pressed(std::string key) const {return m_impl->just_pressed(key);}
bool InputCacheRef::just_released(std::string key) const {return m_impl->just_released(key);}

salmon::MouseState InputCacheRef::get_mouse_state() const {return m_impl->get_mouse_state();}
