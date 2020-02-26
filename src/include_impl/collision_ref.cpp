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
#include "collision_ref.hpp"

#include "actor/collision.hpp"

using salmon::CollisionRef;

CollisionRef::CollisionRef(Collision& impl) : m_impl(&impl) {}

bool CollisionRef::tile() const {return m_impl->tile();}
bool CollisionRef::actor() const {return m_impl->actor();}
bool CollisionRef::mouse() const {return m_impl->mouse();}
bool CollisionRef::none() const {return m_impl->none();}

std::string CollisionRef::my_hitbox() const {return m_impl->my_hitbox();}
std::string CollisionRef::other_hitbox() const {return m_impl->other_hitbox();}
std::string CollisionRef::get_tile_type() const {return m_impl->get_type();}
unsigned CollisionRef::get_actor_id() const {return m_impl->get_actor_id();}

int CollisionRef::get_x() const {return m_impl->get_x();}
int CollisionRef::get_y() const {return m_impl->get_y();}
