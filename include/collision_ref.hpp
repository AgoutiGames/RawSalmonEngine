#ifndef COLLISION_REF_HPP_INCLUDED
#define COLLISION_REF_HPP_INCLUDED

#include <string>

class Collision;

namespace salmon {
    class CollisionRef {
        public:
            CollisionRef(Collision& impl);

            // Checks against tile types
            bool tile() const;
            bool actor() const;
            bool mouse() const;
            bool none() const;

            std::string my_hitbox() const;
            std::string other_hitbox() const;

            unsigned get_actor_id() const;
            std::string get_tile_type() const;

        private:
            Collision* m_impl;
    };
}

#endif // COLLISION_REF_HPP_INCLUDED
