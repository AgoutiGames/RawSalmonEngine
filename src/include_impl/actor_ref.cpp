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

ActorRef::ActorRef(internal::Actor& impl) : m_impl{&impl} {}
ActorRef::ActorRef(internal::Actor* impl) : m_impl{impl} {}

bool ActorRef::good() const {return (m_impl == nullptr) ? false : true ;}

bool ActorRef::animate(std::string anim, Direction dir, float speed) {return m_impl->animate(anim,dir,speed);}
bool ActorRef::set_animation(std::string anim, Direction dir, int frame) {return m_impl->set_animation(anim,dir,frame);}
AnimSignal ActorRef::animate_trigger(std::string anim, Direction dir, float speed) {return m_impl->animate_trigger(anim,dir,speed);}
std::string ActorRef::get_animation() const {return m_impl->get_animation();}
Direction ActorRef::get_direction() const {return m_impl->get_direction();}
std::string ActorRef::get_name() const {return m_impl->get_name();}
std::string ActorRef::get_template_name() const {return m_impl->get_type();}
unsigned ActorRef::get_id() const {return m_impl->get_id();}
bool ActorRef::valid_anim_state(std::string anim, Direction dir) const {return m_impl->valid_anim_state(anim,dir);}

bool ActorRef::move_relative(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {return m_impl->move_relative(x,y,target,my_hitboxes,other_hitboxes,notify);}
bool ActorRef::move_absolute(float x, float y, Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {return m_impl->move_absolute(x,y,target,my_hitboxes,other_hitboxes,notify);}
void ActorRef::move_relative(float x, float y) {m_impl->move_relative(x,y);}
void ActorRef::move_absolute(float x, float y) {m_impl->move_absolute(x,y);}

bool ActorRef::unstuck(Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->unstuck(target,my_hitboxes,other_hitboxes,notify);
}
bool ActorRef::unstuck_along_path(float x, float y,Collidees target, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->unstuck_along_path(x,y,target,my_hitboxes,other_hitboxes,notify);
}

bool ActorRef::check_collision(ActorRef other, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes, bool notify) {
    return m_impl->check_collision(*other.m_impl,my_hitboxes,other_hitboxes,notify);
}

bool ActorRef::separate(ActorRef actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate(*actor.m_impl,my_hitboxes,other_hitboxes,false);
}
bool ActorRef::separate(float x, float y, ActorRef actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate_along_path(x,y,*actor.m_impl,my_hitboxes,other_hitboxes,false);
}
bool ActorRef::separate(float x1, float y1, float x2, float y2, ActorRef actor, const std::vector<std::string>& my_hitboxes, const std::vector<std::string>& other_hitboxes) {
    return m_impl->separate_along_path(x1,y1,x2,y2,*actor.m_impl,my_hitboxes,other_hitboxes);
}

bool ActorRef::on_ground(Collidees target, std::string my_hitbox, const std::vector<std::string>& other_hitboxes, Direction dir, int tolerance) const {return m_impl->on_ground(target,my_hitbox,other_hitboxes,dir,tolerance);}

void ActorRef::set_resize_hitbox(bool mode) {m_impl->set_resize_hitbox(mode);}
bool ActorRef::get_resize_hitbox() const {return m_impl->get_resize_hitbox();}

std::vector<CollisionRef> ActorRef::get_collisions() {
    std::vector<internal::Collision>& temp = m_impl->get_collisions();
    std::vector<CollisionRef> out;
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
Rect ActorRef::get_hitbox(std::string name) const {return internal::make_rect(m_impl->get_hitbox(name));}

DataBlockRef ActorRef::get_data() {return m_impl->get_data();}
