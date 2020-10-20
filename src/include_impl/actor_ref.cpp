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

using namespace salmon;

Actor::Actor(internal::Actor& impl) : m_impl{&impl} {}
Actor::Actor(internal::Actor* impl) : m_impl{impl} {}

bool Actor::good() const {return (m_impl == nullptr) ? false : true ;}

bool Actor::animate(std::string anim, Direction dir, float speed) {return m_impl->animate(anim,dir,speed);}
bool Actor::set_animation(std::string anim, Direction dir, int frame) {return m_impl->set_animation(anim,dir,frame);}
AnimSignal Actor::animate_trigger(std::string anim, Direction dir, float speed) {return m_impl->animate_trigger(anim,dir,speed);}
std::string Actor::get_animation() const {return m_impl->get_animation();}
Direction Actor::get_direction() const {return m_impl->get_direction();}
std::string Actor::get_name() const {return m_impl->get_name();}
std::string Actor::get_template_name() const {return m_impl->get_type();}
unsigned Actor::get_id() const {return m_impl->get_id();}
bool Actor::valid_anim_state(std::string anim, Direction dir) const {return m_impl->valid_anim_state(anim,dir);}

bool Actor::move_relative(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {return m_impl->move_relative(x,y,target,my_hitboxes,other_hitboxes,notify);}
bool Actor::move_absolute(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {return m_impl->move_absolute(x,y,target,my_hitboxes,other_hitboxes,notify);}
void Actor::move_relative(float x, float y) {m_impl->move_relative(x,y);}
void Actor::move_absolute(float x, float y) {m_impl->move_absolute(x,y);}

bool Actor::unstuck(Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->unstuck(target,my_hitboxes,other_hitboxes,notify);
}
bool Actor::unstuck_along_path(float x, float y,Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->unstuck_along_path(x,y,target,my_hitboxes,other_hitboxes,notify);
}

bool Actor::check_collision(Actor other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->check_collision(*other.m_impl,my_hitboxes,other_hitboxes,notify);
}

bool Actor::separate(Actor actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate(*actor.m_impl,my_hitboxes,other_hitboxes,false);
}
bool Actor::separate(float x, float y, Actor actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate_along_path(x,y,*actor.m_impl,my_hitboxes,other_hitboxes,false);
}
bool Actor::separate(float x1, float y1, float x2, float y2, Actor actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate_along_path(x1,y1,x2,y2,*actor.m_impl,my_hitboxes,other_hitboxes);
}

bool Actor::on_ground(Collidees target, std::string my_hitbox, const std::vector<std::string>& other_hitboxes, Direction dir, int tolerance) const {return m_impl->on_ground(target,my_hitbox,other_hitboxes,dir,tolerance);}

void Actor::set_resize_hitbox(bool mode) {m_impl->set_resize_hitbox(mode);}
bool Actor::get_resize_hitbox() const {return m_impl->get_resize_hitbox();}

std::vector<Collision> Actor::get_collisions() {
    std::vector<internal::Collision>& temp = m_impl->get_collisions();
    std::vector<Collision> out;
    for(auto& c : temp) {
        out.emplace_back(c);
    }
    return out;
}
void Actor::clear_collisions() {m_impl->clear_collisions();}
void Actor::register_collisions(bool r) {m_impl->register_collisions(r);}

bool Actor::get_hidden() const {return m_impl->get_hidden();}
void Actor::set_hidden(bool mode) {m_impl->set_hidden(mode);}

std::string Actor::get_layer() const {return m_impl->get_layer();}

int Actor::get_current_anim_frame() const {
    return m_impl->get_animation_tile().get_current_frame();
}
int Actor::get_anim_frame_count() const {
    return m_impl->get_animation_tile().get_frame_count();
}

float Actor::get_x() const {return m_impl->get_x();}
float Actor::get_y() const {return m_impl->get_y();}
PixelRect Actor::get_hitbox(std::string name) const {return internal::make_rect(m_impl->get_hitbox(name));}

DataBlock Actor::get_data() {return m_impl->get_data();}
