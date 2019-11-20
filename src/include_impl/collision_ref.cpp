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
