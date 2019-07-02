/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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
#ifndef CAUSE_HPP_INCLUDED
#define CAUSE_HPP_INCLUDED

#include <string>
#include <SDL.h>

class Actor;
class Tile;

/**
 * @brief Store information of the object which has triggered an event
 *
 * @note Events can be either caused by other actors, keypresses or tiles
 */

class Cause{
    private:
        enum class CauseType{
            none,
            key,
            tile,
            actor
        };

    public:
        Cause();
        Cause(SDL_Keysym key);
        Cause(Tile* tile, std::string my_hitbox, std::string other_hitbox);
        Cause(Actor* actor, std::string my_hitbox, std::string other_hitbox);

        // Checks against tile types
        bool key() const {return type == CauseType::key;}
        bool tile() const {return type == CauseType::tile;}
        bool actor() const {return type == CauseType::actor;}
        bool none() const {return type == CauseType::none;}

        std::string my_hitbox() const {return my_hitbox_name;}
        std::string other_hitbox() const {return other_hitbox_name;}

        // Return cause objects
        const SDL_Keysym get_key() const {return type == CauseType::key ? data.key : SDL_Keysym();}
        Actor* get_actor() const {return type == CauseType::actor ? data.actor : nullptr;}
        Tile* get_tile() const {return type == CauseType::tile ? data.tile : nullptr;}

        std::string get_type() const;

    private:
        CauseType type;

        union{
            SDL_Keysym key;
            Actor* actor;
            /*
            struct{
                Tile* pointer;
                int x;
                int y;
            } tile;*/
            Tile* tile;
        } data;

        std::string my_hitbox_name = "";
        std::string other_hitbox_name = "";

};


#endif // CAUSE_HPP_INCLUDED
