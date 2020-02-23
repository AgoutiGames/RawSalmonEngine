#ifndef COLLISION_REF_HPP_INCLUDED
#define COLLISION_REF_HPP_INCLUDED

#include <string>

class Collision;

namespace salmon {
    /**
     * @brief Reference to collision object. Identifies the collidee of an collision no matter if it's a tile, actor or mouse.
     */
    class CollisionRef {
        public:
            CollisionRef(Collision& impl);

            /// Return true if collidee is a tile
            bool tile() const;
            /// Return true if collidee is an actor
            bool actor() const;
            /// Return true if collidee is the mouse
            bool mouse() const;
            /// Return true if collidee is not defined
            bool none() const;

            /// Return the name of the hitbox of the collision "receiver"
            std::string my_hitbox() const;
            /// Return the name of the hitbox of the collision "giver"
            std::string other_hitbox() const;

            /// Return the actor id of the collidee if it is of type actor, otherwise return 0
            unsigned get_actor_id() const;
            /// Return type value of tile or template name of actor or empty string depending on collidee
            std::string get_tile_type() const;

            /// Return x-pos of actor or tile relative to world origin
            int get_x() const;
            /// Return y-pos of actor or tile relative to world origin
            /// @note Origin of a tile is its upper left corner, origin of an actor is its lower left corner
            int get_y() const;

        private:
            Collision* m_impl;
    };
}

#endif // COLLISION_REF_HPP_INCLUDED
