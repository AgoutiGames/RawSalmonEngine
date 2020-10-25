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
#include "camera.hpp"

#include "map/camera.hpp"

namespace salmon {

Camera::Camera(internal::Camera& impl) : m_impl{&impl} {}

Transform& Camera::get_transform() {return m_impl->get_transform();}

void Camera::bind_actor(Actor actor) {m_impl->bind_actor(actor.m_impl);}
void Camera::unbind_actor() {m_impl->unbind_actor();}

void Camera::bind_map() {m_impl->bind_map();}
void Camera::unbind_map() {m_impl->unbind_map();}

void Camera::update() {m_impl->update();}

} // namespace salmon
