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
#ifndef MAPDATA_HPP_INCLUDED
#define MAPDATA_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "actor/data_block.hpp"
#include "actor/actor.hpp"
#include "map/camera.hpp"
#include "map/layer_collection.hpp"
#include "map/tileset_collection.hpp"
#include "util/game_types.hpp"
#include "util/tinyxml2.h"

class Tile; // forward declaration
class Layer;

template <class Scope>
class Event;

/**
 * @brief Contain, render and update all game map layers
 *
 * This class is the starting point of parsing the Tiled mapfile.
 * It parses the used tilesets and map layers and stores them.
 * It renders layers and tiles.
 */

class MapData {
    public:
        MapData(unsigned screen_w, unsigned screen_h);
        ~MapData();

        tinyxml2::XMLError init_map(std::string filename, SDL_Renderer** renderer);
        bool render() const;
        void update();

        SDL_Renderer* get_renderer() const {return *mpp_renderer;} ///< Return pointer to the SDL_Renderer
        std::string get_file_path() const {return m_base_path;} ///< Return path to the .tmx map file location

        unsigned get_w() const;
        unsigned get_h() const;
        DataBlock& get_data() {return m_data;}
        void register_event(std::pair<std::string, Event<Actor>*> event) {m_events.insert(event);} ///< Link event name with @c Event<Actor>*
        Event<Actor>* get_event(std::string name) const;
        bool check_event(std::string name) const {if(m_events.find(name) != m_events.end()) {return true;} else {return false;}} ///< Return true if event is defined

        const ActorTemplate& get_actor_template(Uint16 gid) const {return m_templates.at(m_gid_to_temp_name.at(gid));}
        ActorTemplate& get_actor_template(std::string actor) {return m_templates[actor];}

        tinyxml2::XMLError add_actor_template(tinyxml2::XMLElement* source, Tile* tile);
        void add_actor_animation(std::string name, AnimationType anim, Direction dir, Tile* tile);
        bool add_actor_hitbox(std::string actor, std::string hitbox, const SDL_Rect& rect);

        TilesetCollection& get_ts_collection() {return m_ts_collection;}
        LayerCollection& get_layer_collection() {return m_layer_collection;}

        tinyxml2::XMLError parse_actor_properties(tinyxml2::XMLElement* source, float& speed, Direction& dir, std::map<Response, Event<Actor>*>& resp);

        bool register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down);
        bool process_key_up(SDL_Event e);
        bool process_key_down(SDL_Event e);
        void process_keys_sustained();

    private:
        // Better delete this after parsing
        tinyxml2::XMLDocument m_mapfile{true, tinyxml2::COLLAPSE_WHITESPACE}; ///< Contains the .tmx map file

        std::string m_base_path = "../data/"; ///< Path to folder where .tmx map files are
        unsigned m_width;  // The map dimensions in tiles
        unsigned m_height;
        SDL_Color m_bg_color;

        DataBlock m_data; ///< This holds custom user values by string

        Camera m_camera;

        //std::vector<Layer> m_layers; ///< Contains all used layers
        LayerCollection m_layer_collection;

        TilesetCollection m_ts_collection;

        std::map<std::string, ActorTemplate> m_templates; ///< List of all actor templates by name
        std::map<Uint16, std::string> m_gid_to_temp_name; ///< List of actor template names by global tile id

        std::map<std::string, Event<Actor>*> m_events; ///< List of all parsed events by name
        Actor* m_player = nullptr;

        std::map<SDL_Keycode, Event<Actor>*> m_key_up;
        std::map<SDL_Keycode, Event<Actor>*> m_key_down;
        std::map<SDL_Scancode, Event<Actor>*> m_key_sustained;

        SDL_Renderer** mpp_renderer = nullptr;
};


#endif // MAPDATA_HPP_INCLUDED
