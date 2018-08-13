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
#include <SDL2/SDL.h>

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
        Cause(Tile* tile, int x = 0, int y = 0);
        Cause(Actor* actor);

        // Checks against tile types
        bool key() const {if(type == CauseType::key) {return true;} else {return false;}}
        bool tile() const {if(type == CauseType::tile) {return true;} else {return false;}}
        bool actor() const {if(type == CauseType::actor) {return true;} else {return false;}}
        bool none() const {if(type == CauseType::none) {return true;} else {return false;}}

        // Return cause objects
        const SDL_Keysym get_key() const {if(type == CauseType::key) {return data.key;} else {return SDL_Keysym();}}
        Actor* get_actor() const {if(type == CauseType::actor) {return data.actor;} else {return nullptr;}}
        Tile* get_tile() const {if(type == CauseType::tile) {return data.tile.pointer;} else {return nullptr;}}

        std::string get_type() const;
        std::pair<int, int> get_pos() const;


    private:
        CauseType type;

        union{
            SDL_Keysym key;
            Actor* actor;
            struct{
                Tile* pointer;
                int x;
                int y;
            } tile;
        } data;

};


#endif // CAUSE_HPP_INCLUDED
