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
#include "include/actor_ref.hpp"

#include "actor/actor.hpp"

using salmon::ActorRef;

ActorRef::ActorRef(Actor& impl) : m_impl{&impl} {}

bool ActorRef::animate(salmon::AnimationType anim, salmon::Direction dir, float speed) {return m_impl->animate(anim,dir,speed);}
bool ActorRef::set_animation(salmon::AnimationType anim, salmon::Direction dir, int frame) {return m_impl->set_animation(anim,dir,frame);}
salmon::AnimSignal ActorRef::animate_trigger(salmon::AnimationType anim, salmon::Direction dir, float speed) {return m_impl->animate_trigger(anim,dir,speed);}
salmon::AnimationType ActorRef::get_animation() const {return m_impl->get_animation();}
salmon::Direction ActorRef::get_direction() const {return m_impl->get_direction();}
std::string ActorRef::get_name() const {return m_impl->get_name();}
std::string ActorRef::get_type() const {return m_impl->get_type();}

bool ActorRef::move(float x_factor, float y_factor, bool absolute) {return m_impl->move(x_factor, y_factor, absolute);}
bool ActorRef::on_ground(salmon::Direction dir, int tolerance) const {return m_impl->on_ground(dir, tolerance);}

double ActorRef::get_angle() const {return m_impl->get_angle();}
void ActorRef::set_angle(double angle) {return m_impl->set_angle(angle);}

float ActorRef::get_x() const {return m_impl->get_x();}
float ActorRef::get_y() const {return m_impl->get_y();}
unsigned ActorRef::get_w() const {return m_impl->get_w();}
unsigned ActorRef::get_h() const {return m_impl->get_h();}
int ActorRef::get_x_center() const {return m_impl->get_x_center();}
int ActorRef::get_y_center() const {return m_impl->get_y_center();}

salmon::DataBlockRef ActorRef::get_data() {return m_impl->get_data();}