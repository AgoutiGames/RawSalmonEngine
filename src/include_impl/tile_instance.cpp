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
#include "tile_instance.hpp"

#include "map/tile.hpp"
#include "util/game_types.hpp"

salmon::TileInstance::TileInstance(::TileInstance tile) : m_impl{std::make_unique<::TileInstance>(tile)} {}

bool salmon::TileInstance::valid() const {return m_impl->valid();}

int salmon::TileInstance::get_x() const {return m_impl->get_x();}
int salmon::TileInstance::get_y() const {return m_impl->get_y();}

int salmon::TileInstance::get_x_center() const {return get_x() + get_w() / 2;}
int salmon::TileInstance::get_y_center() const {return get_y() + get_h() / 2;}

unsigned salmon::TileInstance::get_w() const {return m_impl->get_tile()->get_w();}
unsigned salmon::TileInstance::get_h() const {return m_impl->get_tile()->get_h();}

salmon::Rect salmon::TileInstance::get_hitbox(std::string name) const {return make_rect(m_impl->get_hitbox(name));}

std::string salmon::TileInstance::get_type() const {return m_impl->get_tile()->get_type();}