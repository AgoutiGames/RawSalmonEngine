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
#include "actor_ref.hpp"

#include "actor/actor.hpp"

using salmon::ActorRef;

ActorRef::ActorRef(Actor& impl) : m_impl{&impl} {}
ActorRef::ActorRef(Actor* impl) : m_impl{impl} {}

bool ActorRef::animate(std::string anim, salmon::Direction dir, float speed) {return m_impl->animate(anim,dir,speed);}
bool ActorRef::set_animation(std::string anim, salmon::Direction dir, int frame) {return m_impl->set_animation(anim,dir,frame);}
salmon::AnimSignal ActorRef::animate_trigger(std::string anim, salmon::Direction dir, float speed) {return m_impl->animate_trigger(anim,dir,speed);}
std::string ActorRef::get_animation() const {return m_impl->get_animation();}
salmon::Direction ActorRef::get_direction() const {return m_impl->get_direction();}
std::string ActorRef::get_name() const {return m_impl->get_name();}
std::string ActorRef::get_type() const {return m_impl->get_type();}
unsigned ActorRef::get_id() const {return m_impl->get_id();}

bool ActorRef::move(float x_factor, float y_factor, bool absolute) {return m_impl->move(x_factor, y_factor, absolute);}
bool ActorRef::on_ground(salmon::Direction dir, int tolerance) const {return m_impl->on_ground(dir, tolerance);}

bool ActorRef::scale(float x, float y) {return m_impl->scale(x,y);}
bool ActorRef::scale(float s) {return m_impl->scale(s);}
float ActorRef::get_x_scale() const {return m_impl->get_x_scale();}
float ActorRef::get_y_scale() const {return m_impl->get_y_scale();}

void ActorRef::set_resize_hitbox(bool mode) {m_impl->set_resize_hitbox(mode);}
bool ActorRef::get_resize_hitbox() const {return m_impl->get_resize_hitbox();}

double ActorRef::get_angle() const {return m_impl->get_angle();}
void ActorRef::set_angle(double angle) {return m_impl->set_angle(angle);}

std::vector<salmon::CollisionRef> ActorRef::get_collisions() {
    std::vector<Collision>& temp = m_impl->get_collisions();
    std::vector<salmon::CollisionRef> out;
    for(auto& c : temp) {
        out.emplace_back(c);
    }
    return out;
}
void ActorRef::clear_collisions() {m_impl->clear_collisions();}
void ActorRef::register_collisions(bool r) {m_impl->register_collisions(r);}

bool ActorRef::get_hidden() const {return m_impl->get_hidden();}
void ActorRef::set_hidden(bool mode) {m_impl->set_hidden(mode);}

std::string ActorRef::get_layer() const {return m_impl->get_layer();}

int ActorRef::get_current_anim_frame() const {
    return m_impl->get_animation_tile().get_current_frame();
}
int ActorRef::get_anim_frame_count() const {
    return m_impl->get_animation_tile().get_frame_count();
}

float ActorRef::get_x() const {return m_impl->get_x();}
float ActorRef::get_y() const {return m_impl->get_y();}
unsigned ActorRef::get_w() const {return m_impl->get_w();}
unsigned ActorRef::get_h() const {return m_impl->get_h();}
int ActorRef::get_x_center() const {return m_impl->get_x_center();}
int ActorRef::get_y_center() const {return m_impl->get_y_center();}

salmon::DataBlockRef ActorRef::get_data() {return m_impl->get_data();}
