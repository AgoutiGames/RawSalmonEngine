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
#ifndef ACTOR_HPP_INCLUDED
#define ACTOR_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>

#include "event/actor_event.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

//class ActorEvent;

/**
 * @brief Parse, store and manage all actors
 *
 * @note Actors can only be constructed from parsed @c ActorTemplate
 * @warning @todo Reset @c m_templates and @c m_gid_to_temp_name on map loading
 */

class Actor{

    public:
        Actor(Uint16 tile_id);          ///< Construct actor from tile_id corresponding to an ActorTemplate
        Actor(ActorTemplate& templ);    ///< Construct actor from an ActorTemplate

        tinyxml2::XMLError init_actor(tinyxml2::XMLElement* source);
        bool update();
        bool animate(AnimationType anim, Direction dir);
        void render(int x_cam, int y_cam) const;
        bool move(float x_factor, float y_factor);
        bool process_events();
        void add_event(ActorEvent* event);

        static tinyxml2::XMLError add_template(tinyxml2::XMLElement* source, Uint16 tile_id);
        static void add_animation(std::string name, AnimationType anim, Direction dir, Tile* tile);
        static void initialize();


    private:
        float m_x;
        float m_y;
        unsigned m_width;
        unsigned m_height;
        std::string m_name;
        float m_base_speed;
        // float m_current_speed;

        Behaviour m_AI;
        AnimationType m_anim_state = AnimationType::idle; ///< Currently active animation
        Direction m_direction; ///< Current direction facing
        SDL_Rect m_hitbox;
        std::map<AnimationType, std::map<Direction, Tile>> m_animations; ///< 2D Map which stores all animation tiles

        std::vector<ActorEvent*> m_event_pipeline;

        static std::map<std::string, ActorTemplate> m_templates; ///< List of all actor templates by name
        static std::map<Uint16, std::string> m_gid_to_temp_name; ///< List of actor template names by globa tile id

};


#endif // ACTOR_HPP_INCLUDED
