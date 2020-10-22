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

#include "transform.hpp"
#include "map/tile.hpp"
#include "util/game_types.hpp"

namespace salmon {

TileInstance::TileInstance(internal::TileInstance tile) : m_impl{std::make_unique<internal::TileInstance>(tile)} {}

bool TileInstance::valid() const {return m_impl->valid();}

const Transform& TileInstance::get_transform() const {return m_impl->get_transform();}

Rect TileInstance::get_hitbox(std::string name) const {return m_impl->get_hitbox(name);}

std::string TileInstance::get_type() const {return m_impl->get_tile()->get_type();}

} // namespace salmon
