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
#include "include/camera_ref.hpp"

#include "map/camera.hpp"

using salmon::CameraRef;

CameraRef::CameraRef(Camera& impl) : m_impl{&impl} {}

int CameraRef::get_x() const {return m_impl->x();}
int CameraRef::get_y() const {return m_impl->y();}

void CameraRef::set_x(int x) {m_impl->set_x(x);}
void CameraRef::set_y(int y) {m_impl->set_y(y);}

void CameraRef::bind_actor(ActorRef actor) {m_impl->bind_actor(actor.m_impl);}
void CameraRef::unbind_actor() {m_impl->unbind_actor();}

void CameraRef::bind_map() {m_impl->bind_map();}
void CameraRef::unbind_map() {m_impl->unbind_map();}
